#include "include/firebase_analytics/firebase_analytics_plugin.h"

#include <flutter/plugin_registrar_windows.h>

#include "firebase_analytics_plugin.h"

void FirebaseAnalyticsPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  firebase_analytics::FirebaseAnalyticsPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
