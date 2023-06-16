#include "firebase_analytics_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

#include <Ras.h>
#include <RasError.h>
#include <vector>
#include <iostream>


// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <Firebase/Firebase.h>

#include <firebase_core/FLTFirebasePluginRegistry.h>

#include <memory>
#include <sstream>

char *const kFLTFirebaseAnalyticsName = "name";
char *const kFLTFirebaseAnalyticsValue = "value";
char *const kFLTFirebaseAnalyticsEnabled = "enabled";
char *const kFLTFirebaseAnalyticsEventName = "eventName";
char *const kFLTFirebaseAnalyticsParameters = "parameters";
char *const kFLTFirebaseAnalyticsAdStorageConsentGranted = "adStorageConsentGranted";
char *const kFLTFirebaseAnalyticsStorageConsentGranted = "analyticsStorageConsentGranted";
char *const kFLTFirebaseAnalyticsUserId = "userId";

char *const FLTFirebaseAnalyticsChannelName = "plugins.flutter.io/firebase_analytics";


namespace firebase_analytics {

// static
void FirebaseAnalyticsPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "firebase_analytics",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<FirebaseAnalyticsPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

FirebaseAnalyticsPlugin::FirebaseAnalyticsPlugin() {}

FirebaseAnalyticsPlugin::~FirebaseAnalyticsPlugin() {}

void FirebaseAnalyticsPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  }
  else if (method_call.method_name().compare("Analytics#logEvent") == 0) {
    logEvent(arguments, result);
  }
  else if (method_call.method_name().compare("Analytics#setUserId") == 0) {
    setUserId(arguments, result);
  }
  else if (method_call.method_name().compare("Analytics#setUserProperty") == 0){
    setUserProperty(arguments, result);
  }
  else if (method_call.method_name().compare("Analytics#setAnalyticsCollectionEnabled") == 0) {
    setAnalyticsCollectionEnabled(arguments, result);
  }
  else if (method_call.method_name().compare("Analytics#resetAnalyticsData") == 0) {
    resetAnalyticsData(arguments, result);
  }
  else if (method_call.method_name().compare("Analytics#setConsent") == 0) {
    setConsent(arguments, result);
  }
  else if (method_call.method_name().compare("Analytics#setDefaultEventParameters") == 0) {
    setDefaultEventParameters(arguments, result);
  }
  else if (method_call.method_name().compare("Analytics#getAppInstanceId") == 0) {
    getAppInstanceId(arguments, result);
  }
  else {
    result->NotImplemented();
  }

  
void  logEvent(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  std::string eventName = std::get<std::string>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsEventName))); 
  id parameterMap = arguments[kFLTFirebaseAnalyticsParameters];

  if (parameterMap != [NSNull null]) {
    FIRAnalytics::logEventWithName:eventName parameters:parameterMap;
  } else {
    FIRAnalytics::logEventWithName:eventName parameters:nil;
  }

  result->Success();
}

void  setUserId(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  std::string userId = std::get<std::string>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsUserId))); 
  FIRAnalytics::setUserID:[userId isKindOfClass:[NSNull class]] ? nil : userId;

  result->Success();
}

void  setUserProperty(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  std::string name = std::get<std::string>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsName)));  
  std::string value = std::get<std::string>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsValue))); 
  FIRAnalytics::setUserPropertyString:[value isKindOfClass:[NSNull class]] ? nil : value
                              forName:name;
  result->Success();
}

void  setAnalyticsCollectionEnabled(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  NSNumber *enabled = arguments[kFLTFirebaseAnalyticsEnabled];
  FIRAnalytics::setAnalyticsCollectionEnabled:[enabled boolValue];
   result->Success();
}

void  resetAnalyticsData(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  FIRAnalytics::resetAnalyticsData;
  result->Success();
}

void  setConsent(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  NSNumber *adStorageGranted = arguments[kFLTFirebaseAnalyticsAdStorageConsentGranted];
  NSNumber *analyticsStorageGranted = arguments[kFLTFirebaseAnalyticsStorageConsentGranted];
  NSMutableDictionary<FIRConsentType, FIRConsentStatus> *parameters =
      [[NSMutableDictionary alloc] init];

  if (adStorageGranted != nil) {
    parameters[FIRConsentTypeAdStorage] =
        [adStorageGranted boolValue] ? FIRConsentStatusGranted : FIRConsentStatusDenied;
  }
  if (analyticsStorageGranted != nil) {
    parameters[FIRConsentTypeAnalyticsStorage] =
        [analyticsStorageGranted boolValue] ? FIRConsentStatusGranted : FIRConsentStatusDenied;
  }

  FIRAnalytics::setConsent:parameters;
  result->Success();
}

void  setDefaultEventParameters(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  FIRAnalytics::setDefaultEventParameters:arguments;
  result->Success();
}

void  getAppInstanceIdWithMethodCallResult:std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  std::string *appInstanceID = FIRAnalytics::appInstanceID;
  result->Success(appInstanceID);
}


}  // namespace firebase_analytics
