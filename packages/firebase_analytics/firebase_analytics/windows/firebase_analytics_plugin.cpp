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

#include "firebase/analytics.h"


#include <memory>
#include <sstream>


const char* kFLTFirebaseAnalyticsName = "name";
const char* kFLTFirebaseAnalyticsValue = "value";
const char* kFLTFirebaseAnalyticsEnabled = "enabled";
const char* kFLTFirebaseAnalyticsEventName = "eventName";
const char* kFLTFirebaseAnalyticsParameters = "parameters";
const char* kFLTFirebaseAnalyticsAdStorageConsentGranted = "adStorageConsentGranted";
const char* kFLTFirebaseAnalyticsStorageConsentGranted = "analyticsStorageConsentGranted";
const char* kFLTFirebaseAnalyticsUserId = "userId";

const char* FLTFirebaseAnalyticsChannelName = "plugins.flutter.io/firebase_analytics";


namespace firebase_analytics {

void  logEvent(flutter::EncodableMap& args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const std::string* eventName = std::get_if<std::string>(&args[flutter::EncodableValue(kFLTFirebaseAnalyticsEventName)]);
	const flutter::EncodableMap* parameterMap = std::get_if<flutter::EncodableMap>(&args[flutter::EncodableValue(kFLTFirebaseAnalyticsParameters)]);
	if (parameterMap != nullptr) {
		std::vector<firebase::analytics::Parameter> params;
		for (auto it = parameterMap->begin(); it != parameterMap->end(); ++it) {
			std::string key;
			firebase::Variant var;
			if (!std::holds_alternative<std::string>(it->first)) {
				continue;
		    }
			key = std::get<std::string>(it->first);
			if (std::holds_alternative<std::monostate>(it->second)) {
				var = firebase::Variant("null");
			} 
			else if (std::holds_alternative<bool>(it->second)) {
				var = firebase::Variant(std::get<bool>(it->second));
			}
			else if (std::holds_alternative<int32_t>(it->second)) {
				var = firebase::Variant(std::get<int32_t>(it->second));
			}
			else if (std::holds_alternative<int64_t>(it->second)) {
				var = firebase::Variant(std::get<int64_t>(it->second));
			}
			else if (std::holds_alternative<double>(it->second)) {
				var = firebase::Variant(std::get<double>(it->second));
			}
			else if (std::holds_alternative<std::string>(it->second)) {
				var = firebase::Variant(std::get<std::string>(it->second));
			}
			else if (std::holds_alternative<std::vector<uint8_t>>(it->second)) {
				var = firebase::Variant(std::get<std::vector<uint8_t>>(it->second));
			}
			else if (std::holds_alternative<std::vector<int32_t>>(it->second)) {
				var = firebase::Variant(std::get<std::vector<int32_t>>(it->second));
			}
			else if (std::holds_alternative<std::vector<int64_t>>(it->second)) {
				var = firebase::Variant(std::get<std::vector<int64_t>>(it->second));
			}
			else if (std::holds_alternative<std::vector<float>>(it->second)) {
				var = firebase::Variant(std::get<std::vector<float>>(it->second));
			}
			else if (std::holds_alternative<std::vector<double>>(it->second)) {
				var = firebase::Variant(std::get<std::vector<double>>(it->second));
			}
			else if (std::holds_alternative<flutter::EncodableList>(it->second)) {
				//var = firebase::Variant(std::get<lutter::EncodableList>(it->second));
			}
			else if (std::holds_alternative<flutter::EncodableMap>(it->second)) {
				//var = firebase::Variant(std::get<flutter::EncodableMap>(it->second));
			}
			params.push_back(firebase::analytics::Parameter(key.c_str(), var));
		}
		firebase::analytics::LogEvent(eventName->c_str(), &params[0], params.size());
	}
	else {
		firebase::analytics::LogEvent(eventName->c_str());
	}

	result->Success();
}

void  setUserId(flutter::EncodableMap& args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const std::string* userId = std::get_if<std::string>(&args[flutter::EncodableValue(kFLTFirebaseAnalyticsUserId)]);
	firebase::analytics::SetUserId(userId != nullptr ? userId->c_str() : nullptr);

	result->Success();
}

void  setUserProperty(flutter::EncodableMap& args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const std::string* name = std::get_if<std::string>(&args[flutter::EncodableValue(kFLTFirebaseAnalyticsName)]);
	const std::string* value = std::get_if<std::string>(&args[flutter::EncodableValue(kFLTFirebaseAnalyticsValue)]);
	firebase::analytics::SetUserProperty(name->c_str(), value != nullptr ? value->c_str() : nullptr);

	result->Success();
}

void  setAnalyticsCollectionEnabled(flutter::EncodableMap& args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const bool* enabled = std::get_if<bool>(&args[flutter::EncodableValue(kFLTFirebaseAnalyticsEnabled)]);
	firebase::analytics::SetAnalyticsCollectionEnabled(*enabled);

	result->Success();
}

void  resetAnalyticsData(flutter::EncodableMap& args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	firebase::analytics::ResetAnalyticsData();

	result->Success();
}

void  setConsent(flutter::EncodableMap& args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	const int* adStorageGranted = std::get_if<int>(&args[flutter::EncodableValue(kFLTFirebaseAnalyticsAdStorageConsentGranted)]);
	const int* analyticsStorageGranted = std::get_if<int>(&args[flutter::EncodableValue(kFLTFirebaseAnalyticsStorageConsentGranted)]);
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

void  setDefaultEventParameters(flutter::EncodableMap& args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
	//firebase::analytics::setDefaultEvent(args);

	result->Success();
}

void  getAppInstanceId(flutter::EncodableMap& args, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
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
	auto arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
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
