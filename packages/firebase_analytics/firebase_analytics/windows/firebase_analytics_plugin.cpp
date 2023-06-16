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

#include <Firebase/analytics.h>

#include <firebase_core/firebase_core_plugin_c_api.h>

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
          registrar->messenger(), FLTFirebaseAnalyticsChannelName,
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
  std::string* eventName = std::get_if<std::string>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsEventName))); 
  auto* parameterMap = std::get_if<std::map<std::string, std::string>>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsParameters)));
  if (parameterMap != null) {
    for(auto it = parameterMap.begin();it != parameterMap.end(); ++it){
      firebase::analytics::LogEvent(eventName, firebase::analytics::Parameter(it->first, it->second));
    }
  } else {
    firebase::analytics::LogEvent(eventName);
  }

  result->Success();
}

void  setUserId(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  std::string* userId = std::get_if<std::string>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsUserId))); 
  firebase::analytics::SetUserId(userId != null ? userId.c_str() : null);

  result->Success();
}

void  setUserProperty(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  std::string* name = std::get_if<std::string>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsName)));  
  std::string* value = std::get_if<std::string>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsValue))); 
  firebase::analytics::SetUserProperty(name.c_str(), value != null ? value.c_str() : null);

  result->Success();
}

void  setAnalyticsCollectionEnabled(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  bool* enabled = std::get_if<bool>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsEnabled)));  
  firebase::analytics::setAnalyticsCollectionEnabled(*enabled);

  result->Success();
}

void  resetAnalyticsData(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  firebase::analytics::resetAnalyticsData();

  result->Success();
}

void  setConsent(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  int* adStorageGranted =  std::get_if<int>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsAdStorageConsentGranted)));  
  int* analyticsStorageGranted = std::get_if<int>(args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsStorageConsentGranted)));  
  std::map<ConsentType, ConsentStatus> parameters;
  if (adStorageGranted != null) {
    parameters[firebase::analytics::kConsentTypeAdStorage] =
        adStorageGranted != 0 ? firebase::analytics::kConsentStatusGranted : firebase::analytics::kConsentStatusDenied;
  }
  if (analyticsStorageGranted != null) {
    parameters[firebase::analytics::kConsentTypeAnalyticsStorage] =
        analyticsStorageGranted != 0 ? firebase::analytics::kConsentStatusGranted : firebase::analytics::kConsentStatusDenied;
  }

  firebase::analytics::setConsent(parameters);

  result->Success();
}

void  setDefaultEventParameters(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  //firebase::analytics::setDefaultEventParameters:arguments;

  result->Success();
}

void  getAppInstanceIdWithMethodCallResult:std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  std::string appInstanceID = firebase::analytics::GetAnalyticsInstanceId().wait().get();

  result->Success(appInstanceID);
}


}  // namespace firebase_analytics
