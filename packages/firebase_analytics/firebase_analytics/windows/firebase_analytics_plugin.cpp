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

#include <memory>
#include <sstream>

// Big Thanks to Qv2ray opensource project
std::string encodeableMapToString(const flutter::EncodableMap* args){
    auto proxy = std::get<std::string>(args->at(flutter::EncodableValue("proxy")));
    return proxy;
}
 
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
  else if (method_call.method_name().compare("cleanSystemProxy") == 0) {
      //cleanSystemProxy();
      result->Success();
  }
  else if (method_call.method_name().compare("setSystemProxy") == 0) {
      auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
      //setSystemProxy(arguments);
      result->Success();
  }
  else if (method_call.method_name().compare("getSystemProxyEnable") == 0){
      auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
      bool enable = false;//getSystemProxyEnable(arguments);
      result->Success(flutter::EncodableValue(enable ? "true" : "false"));
  }
  else {
    result->NotImplemented();
  }
}

}  // namespace firebase_analytics
