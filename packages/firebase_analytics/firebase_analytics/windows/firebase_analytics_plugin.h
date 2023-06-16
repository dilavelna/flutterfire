#ifndef FLUTTER_PLUGIN_FIREBASE_ANALYTICS_PLUGIN_H_
#define FLUTTER_PLUGIN_FIREBASE_ANALYTICS_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>

namespace firebase_analytics {

class FirebaseAnalyticsPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  FirebaseAnalyticsPlugin();

  virtual ~FirebaseAnalyticsPlugin();

  // Disallow copy and assign.
  FirebaseAnalyticsPlugin(const FirebaseAnalyticsPlugin&) = delete;
  FirebaseAnalyticsPlugin& operator=(const FirebaseAnalyticsPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace firebase_analytics

#endif  // FLUTTER_PLUGIN_FIREBASE_ANALYTICS_PLUGIN_H_
