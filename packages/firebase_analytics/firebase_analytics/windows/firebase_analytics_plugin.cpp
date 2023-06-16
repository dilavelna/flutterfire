#include "firebase_analytics_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

#include <Ras.h>
#include <RasError.h>
#include <vector>
#include <iostream>
#include <variant>


// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <firebase/analytics.h>


#include <memory>
#include <sstream>

#pragma comment(lib, "firebase_app.lib")
#pragma comment(lib, "firebase_analytics.lib")

char* const kFLTFirebaseAnalyticsName = "name";
char* const kFLTFirebaseAnalyticsValue = "value";
char* const kFLTFirebaseAnalyticsEnabled = "enabled";
char* const kFLTFirebaseAnalyticsEventName = "eventName";
char* const kFLTFirebaseAnalyticsParameters = "parameters";
char* const kFLTFirebaseAnalyticsAdStorageConsentGranted = "adStorageConsentGranted";
char* const kFLTFirebaseAnalyticsStorageConsentGranted = "analyticsStorageConsentGranted";
char* const kFLTFirebaseAnalyticsUserId = "userId";

char* const FLTFirebaseAnalyticsChannelName = "plugins.flutter.io/firebase_analytics";


namespace firebase_analytics {

void  logEvent(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const std::string* eventName = std::get_if<std::string>(&args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsEventName)));
	/*const std::map<std::string, std::string>* parameterMap = std::get_if<std::map<std::string, std::string>>(&args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsParameters)));
	if (parameterMap != nullptr) {
		std::vector<firebase::analytics::Parameter> params;
		for (auto it = parameterMap->begin(); it != parameterMap->end(); ++it) {
			params.push_back(firebase::analytics::Parameter(it->first.c_str(), firebase::Variant(it->second)));
		}
		firebase::analytics::LogEvent(eventName->c_str(), &params[0], params.size());
	}
	else {
		firebase::analytics::LogEvent(eventName->c_str());
	}*/

	result->Success();
}

void  setUserId(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const std::string* userId = std::get_if<std::string>(&args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsUserId)));
	firebase::analytics::SetUserId(userId != nullptr ? userId->c_str() : nullptr);

	result->Success();
}

void  setUserProperty(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const std::string* name = std::get_if<std::string>(&args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsName)));
	const std::string* value = std::get_if<std::string>(&args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsValue)));
	firebase::analytics::SetUserProperty(name->c_str(), value != nullptr ? value->c_str() : nullptr);

	result->Success();
}

void  setAnalyticsCollectionEnabled(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const bool* enabled = std::get_if<bool>(&args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsEnabled)));
	firebase::analytics::SetAnalyticsCollectionEnabled(*enabled);

	result->Success();
}

void  resetAnalyticsData(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	firebase::analytics::ResetAnalyticsData();

	result->Success();
}

void  setConsent(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const int* adStorageGranted = std::get_if<int>(&args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsAdStorageConsentGranted)));
	const int* analyticsStorageGranted = std::get_if<int>(&args->at(flutter::EncodableValue(kFLTFirebaseAnalyticsStorageConsentGranted)));
	std::map<firebase::analytics::ConsentType, firebase::analytics::ConsentStatus> parameters;
	if (adStorageGranted != nullptr) {
		parameters[firebase::analytics::kConsentTypeAdStorage] =
			adStorageGranted != 0 ? firebase::analytics::kConsentStatusGranted : firebase::analytics::kConsentStatusDenied;
	}
	if (analyticsStorageGranted != nullptr) {
		parameters[firebase::analytics::kConsentTypeAnalyticsStorage] =
			analyticsStorageGranted != 0 ? firebase::analytics::kConsentStatusGranted : firebase::analytics::kConsentStatusDenied;
	}

	firebase::analytics::SetConsent(parameters);

	result->Success();
}

void  setDefaultEventParameters(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	//firebase::analytics::setDefaultEvent(args);

	result->Success();
}

void  getAppInstanceId(const flutter::EncodableMap* args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const std::string* appInstanceID = firebase::analytics::GetAnalyticsInstanceId().result();

	result->Success(appInstanceID);
}
// static
void FirebaseAnalyticsPlugin::RegisterWithRegistrar(
	flutter::PluginRegistrarWindows* registrar) {
	auto channel =
		std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
			registrar->messenger(), FLTFirebaseAnalyticsChannelName,
			&flutter::StandardMethodCodec::GetInstance());

	auto plugin = std::make_unique<FirebaseAnalyticsPlugin>();

	channel->SetMethodCallHandler(
		[plugin_pointer = plugin.get()](const auto& call, auto result) {
			plugin_pointer->HandleMethodCall(call, std::move(result));
		});

	registrar->AddPlugin(std::move(plugin));
}

FirebaseAnalyticsPlugin::FirebaseAnalyticsPlugin() {}

FirebaseAnalyticsPlugin::~FirebaseAnalyticsPlugin() {}

void FirebaseAnalyticsPlugin::HandleMethodCall(
	const flutter::MethodCall<flutter::EncodableValue>& method_call,
	std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
	if (method_call.method_name().compare("getPlatformVersion") == 0) {
		std::ostringstream version_stream;
		version_stream << "Windows ";
		if (IsWindows10OrGreater()) {
			version_stream << "10+";
		}
		else if (IsWindows8OrGreater()) {
			version_stream << "8";
		}
		else if (IsWindows7OrGreater()) {
			version_stream << "7";
		}
		result->Success(flutter::EncodableValue(version_stream.str()));
	}
	else if (method_call.method_name().compare("Analytics#logEvent") == 0) {
		logEvent(arguments, std::move(result));
	}
	else if (method_call.method_name().compare("Analytics#setUserId") == 0) {
		setUserId(arguments, std::move(result));
	}
	else if (method_call.method_name().compare("Analytics#setUserProperty") == 0) {
		setUserProperty(arguments, std::move(result));
	}
	else if (method_call.method_name().compare("Analytics#setAnalyticsCollectionEnabled") == 0) {
		setAnalyticsCollectionEnabled(arguments, std::move(result));
	}
	else if (method_call.method_name().compare("Analytics#resetAnalyticsData") == 0) {
		resetAnalyticsData(arguments, std::move(result));
	}
	else if (method_call.method_name().compare("Analytics#setConsent") == 0) {
		setConsent(arguments, std::move(result));
	}
	else if (method_call.method_name().compare("Analytics#setDefaultEventParameters") == 0) {
		setDefaultEventParameters(arguments, std::move(result));
	}
	else if (method_call.method_name().compare("Analytics#getAppInstanceId") == 0) {
		getAppInstanceId(arguments, std::move(result));
	}
	else {
		result->NotImplemented();
	}
}

}  // namespace firebase_analytics
