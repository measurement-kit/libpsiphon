// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.

#include <iostream>

#include "mkpsiphon.h"

#ifdef _WIN32
#define LIBPSIPHON "libpsiphontunnel.dll"
#elif defined __APPLE__
#define LIBPSIPHON "libpsiphontunnel.dylib"
#else
#define LIBPSIPHON "libpsiphontunnel.so"
#endif

int main() {
  mkpsiphon_settings_uptr settings{mkpsiphon_settings_new_nonnull()};
  mkpsiphon_settings_set_timeout(settings.get(), 10);
  mkpsiphon_settings_set_platform(
      settings.get(), "OSName_OSVersion_com.example.libpsiphon");
  mkpsiphon_settings_set_network_id(settings.get(), "TEST");
  mkpsiphon_settings_set_config_file_path(
      settings.get(), "psiphon-config.json");
  mkpsiphon_settings_set_dynamic_library_path(settings.get(), LIBPSIPHON);
  mkpsiphon_results_uptr results{
      mkpsiphon_settings_perform_nonnull(settings.get())};
  if (mkpsiphon_results_get_error_code(results.get()) != 0) {
    std::clog << "Psiphon failed to connect with error: "           //
              << mkpsiphon_results_get_error_reason(results.get())  //
              << std::endl;
    exit(EXIT_FAILURE);
  }
  std::clog << "Psiphon connected in "                              //
            << mkpsiphon_results_get_bootstrap_time(results.get())  //
            << " seconds" << std::endl;
}
