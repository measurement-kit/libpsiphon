// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.
#ifndef MEASUREMENT_KIT_MKPSIPHON_H
#define MEASUREMENT_KIT_MKPSIPHON_H

#include <stdint.h>
#include <stdlib.h>

/// @file mkpsiphon.h. This file contains the measurement-kit code
/// implementing a psiphon bootstrap test.

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// mkpsiphon_settings_t contains psiphon settings.
typedef struct mkpsiphon_settings mkpsiphon_settings_t;

/// mkpsiphon_results_t contains psiphon results.
typedef struct mkpsiphon_results mkpsiphon_results_t;

/// mkpsiphon_settings_new_nonnull creates default settings. This function
/// will always return a valid pointer and abort on malloc failure.
mkpsiphon_settings_t *mkpsiphon_settings_new_nonnull(void);

/// mkpsiphon_settings_set_dynamic_library_path sets the library path to the
/// psiphon dynamic library. It aborts if passed null pointers.
void mkpsiphon_settings_set_dynamic_library_path(
    mkpsiphon_settings_t *settings, const char *dynamic_library_path);

/// mkpsiphon_settings_set_timeout sets the psiphon client timeout to be @p
/// timeout seconds. This function aborts if @p settings is null.
void mkpsiphon_settings_set_timeout(
    mkpsiphon_settings_t *settings, int64_t timeout);

/// mkpsiphon_settings_set_platform sets the platform to be used by
/// psiphon. This function aborts if passed null pointers.
void mkpsiphon_settings_set_platform(
    mkpsiphon_settings_t *settings, const char *platform);

/// mkpsiphon_settings_set_network_id sets the network ID. This
/// function will abort if passed null pointers.
void mkpsiphon_settings_set_network_id(
    mkpsiphon_settings_t *settings, const char *network_id);

/// mkpsiphon_settings_set_config_file_path sets the config file path. This
/// function will abort if passed null pointers.
void mkpsiphon_settings_set_config_file_path(
    mkpsiphon_settings_t *settings, const char *config_file_path);

/// mkpsiphon_settings_perform_nonnull performs a psiphon connectivity
/// test. This function never returns a null pointer. This function will
/// abort if the @p settings argument is a null pointer.
mkpsiphon_results_t *mkpsiphon_settings_perform_nonnull(
    const mkpsiphon_settings_t *settings);

/// mkpsiphon_settings_delete destroys @p settings. This function will
/// gracefully handle a null @p settings arg.
void mkpsiphon_settings_delete(mkpsiphon_settings_t *settings);

/// mkpsiphon_results_get_error_code returns the error that occurred during
/// psiphon's bootstrap. A return value of zero indicates that no
/// error did actually occur. This function aborts if @p results is null.
int64_t mkpsiphon_results_get_error_code(const mkpsiphon_results_t *results);

/// mkpsiphon_results_get_error_reason returns the reason why psiphon
/// bootsteap failed. The returned string is owned by @p results. This
/// function will abort if the @p results argument is null.
const char *mkpsiphon_results_get_error_reason(
    const mkpsiphon_results_t *results);

/// mkpsiphon_results_get_bootstrap_time returns the bootstrap time. This
/// function will abort if @p result is a null pointer.
double mkpsiphon_results_get_bootstrap_time(
    const mkpsiphon_results_t *results);

/// mkpsiphon_results_delete destroys @p results. This function will
/// gracefully handle a null @p results arg.
void mkpsiphon_results_delete(mkpsiphon_results_t *results);

#ifdef __cplusplus
}  // extern "C"

#include <memory>

/// mkpsiphon_settings_deleter is a deleter for mkpsiphon_settings_t.
struct mkpsiphon_settings_deleter {
  void operator()(mkpsiphon_settings_t *s) { mkpsiphon_settings_delete(s); }
};

/// mkpsiphon_settings_uptr is a unique pointer to a mkpsiphon_settings_t.
using mkpsiphon_settings_uptr = std::unique_ptr<
    mkpsiphon_settings_t, mkpsiphon_settings_deleter>;

/// mkpsiphon_results_deleter is a deleter for mkpsiphon_results_t.
struct mkpsiphon_results_deleter {
  void operator()(mkpsiphon_results_t *s) { mkpsiphon_results_delete(s); }
};

/// mkpsiphon_results_uptr is a unique pointer to a mkpsiphon_results_t.
using mkpsiphon_results_uptr = std::unique_ptr<
    mkpsiphon_results_t, mkpsiphon_results_deleter>;

// MKPSIPHON_INLINE_IMPL allows to include the implementation inline.
#ifdef MKPSIPHON_INLINE_IMPL

#include <fstream>
#include <iostream>
#include <string>

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include "json.hpp"

// MKPSIPHON_TRACE allows to override variables in unit tests.
#if !defined MKPSIPHON_TRACE
#if !defined NDEBUG
#define MKPSIPHON_TRACE(Tag, Var) std::clog << #Tag << ": " << Var << std::endl
#else
#define MKPSIPHON_TRACE(Tag, Var)  // Nothing
#endif  // !NDEBUG
#endif  // !MKPSIPHON_TRACE

// mkpsiphon_settings contains psiphon tunnel settings.
struct mkpsiphon_settings {
  // dynamic_library_path is the full path to psiphon dynamic library.
  std::string dynamic_library_path;

  // timeout is the bootstrap timeout in seconds.
  int64_t timeout = 10;

  // platform is the identifier of the platform where we're running.
  std::string platform;

  // network_id is the psiphon network identifier.
  std::string network_id;

  // config_file_path is the path to psiphon config file.
  std::string config_file_path;
};

// mkpsiphon_results contains the bootstrap results.
struct mkpsiphon_results {
  // error_code contains the error code. By default we set it to a nonzero
  // value indicating that a failure occurs. Zero means success.
  int64_t error_code = INT64_MAX;

  // error_reason is the reason why the test failed. This field should be
  // empty when no error has occurred.
  std::string error_reason;

  // bootstrap_time is the time that it took psiphon to bootstrap. This
  // field should be zero in case of failure.
  double bootstrap_time = 0.0;
};

mkpsiphon_settings_t *mkpsiphon_settings_new_nonnull() {
  return new mkpsiphon_settings_t;
}

#ifndef MKPSIPHON_ABORT
#define MKPSIPHON_ABORT() abort()
#endif

void mkpsiphon_settings_set_dynamic_library_path(
    mkpsiphon_settings_t *settings, const char *dynamic_library_path) {
  if (settings == nullptr || dynamic_library_path == nullptr) {
    MKPSIPHON_ABORT();
  }
  settings->dynamic_library_path = dynamic_library_path;
}

void mkpsiphon_settings_set_timeout(
    mkpsiphon_settings_t *settings, int64_t timeout) {
  if (settings == nullptr) {
    MKPSIPHON_ABORT();
  }
  settings->timeout = timeout;
}

void mkpsiphon_settings_set_platform(
    mkpsiphon_settings_t *settings, const char *platform) {
  if (settings == nullptr || platform == nullptr) {
    MKPSIPHON_ABORT();
  }
  settings->platform = platform;
}

void mkpsiphon_settings_set_network_id(
    mkpsiphon_settings_t *settings, const char *network_id) {
  if (settings == nullptr || network_id == nullptr) {
    MKPSIPHON_ABORT();
  }
  settings->network_id = network_id;
}

void mkpsiphon_settings_set_config_file_path(
    mkpsiphon_settings_t *settings, const char *config_file_path) {
  if (settings == nullptr || config_file_path == nullptr) {
    MKPSIPHON_ABORT();
  }
  settings->config_file_path = config_file_path;
}

// TODO(bassosimone): write equivalent code for Windows.
//
// TODO(bassosimone): understand whether this strategy works for iOS.
#ifndef _WIN32
// mkpsiphon_dso_closer closes an open DSO handle.
struct mkpsiphon_dso_closer {
  void operator()(void *handle) {
    // See https://stackoverflow.com/questions/11412943
    if (handle != nullptr) {
      dlclose(handle);
    }
  }
};
// mkpsiphon_dso_uptr is a unique pointer to a DSO handle.
using mkpsiphon_dso_uptr = std::unique_ptr<void, mkpsiphon_dso_closer>;
#endif

// TODO(bassosimone): psiphon code seems to have a singleton semantic. I am
// not sure whether we should or should not enforce this semantic in here.

// mkpsiphon_start_t is the type of the psiphon start function. Compared to
// the one exported to Go, we've added constness constraints.
typedef const char *(mkpsiphon_start_t)(const char *, const char *,
                                        const char *, const char *,
                                        int64_t);

// mkpsiphon_stop_t is the type of the psiphon stop function.
typedef void (mkpsiphon_stop_t)(void);

mkpsiphon_results_t *mkpsiphon_settings_perform_nonnull(
    const mkpsiphon_settings_t *settings) {
  if (settings == nullptr) {
    MKPSIPHON_ABORT();
  }
  mkpsiphon_results_uptr res{new mkpsiphon_results_t};
  mkpsiphon_start_t *start = nullptr;
  mkpsiphon_stop_t *stop = nullptr;
  mkpsiphon_dso_uptr handle;

  // System dependent part: open libpsiphontunnel and fetch the pointers.
  {
#ifdef _WIN32
    res->error_reason = "Windows is not supported";
    return res.release();
#else
    MKPSIPHON_TRACE(dynamic_library_path, settings->dynamic_library_path);
    {
      void *ptr = dlopen(settings->dynamic_library_path.c_str(), RTLD_NOW);
      MKPSIPHON_TRACE(dlopen, ptr);
      handle.reset(ptr);
    }
    if (handle == nullptr) {
      res->error_reason = "Cannot open psiphon shared library";
      return res.release();
    }
    start = (mkpsiphon_start_t *)dlsym(handle.get(), "psiphon_tunnel_start");
    MKPSIPHON_TRACE(dlsym_psiphon_tunnel_start, start);
    if (start == nullptr) {
      res->error_reason = "Cannot find 'psiphon_tunnel_start' in library";
      return res.release();
    }
    stop = (mkpsiphon_stop_t *)dlsym(handle.get(), "psiphon_tunnel_stop");
    MKPSIPHON_TRACE(dlsym_psiphon_tunnel_stop, stop);
    if (stop == nullptr) {
      res->error_reason = "Cannot find 'psiphon_tunnel_stop' in library";
      return res.release();
    }
#endif
  }

  // Read configuration file and store it into a string.
  std::string config;
  {
    MKPSIPHON_TRACE(config_file_path, settings->config_file_path);
    std::ifstream filep;
    filep.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      filep.open(settings->config_file_path);
      config = std::string{std::istreambuf_iterator<char>{filep},
                           std::istreambuf_iterator<char>{}};
      // Important!!! The following line MUST NOT be uncommented because
      // we don't want to leak the psiphon config inside travis logs.
      //MKPSIPHON_TRACE(config_string, config);
      //
    } catch (const std::exception &) {
      res->error_reason = "Cannot read config file";
      return res.release();
    }
  }

  // Start psiphon tunnel and then immediately stop it. This is sufficient
  // to check whether psiphon can bootstrap. If and when we want to also
  // use psiphon to submit results etc, we can introduce a better API here
  // that actually keeps the tunnel open for as long as we need.
  {
    std::string serialised;
    {
      // Implementation note: if the timeout is negative then psiphon will
      // just fail connecting immediately without misbehaving. In particular
      // the returned failure is related to Go's context. For this reason
      // I have decided that it's fine to void checking for negative timeout.
      const char *str = (*start)(config.c_str(), "", settings->platform.c_str(),
                                 settings->network_id.c_str(), settings->timeout);
      MKPSIPHON_TRACE(start_rv, str);
      if (str == nullptr) {
        // This should not happen. I don't even know whether after this we can
        // assume that psiphon is in a consistent state. So, let's just die.
        MKPSIPHON_ABORT();
      }
      serialised = str;
      (*stop)();
    }
    try {
      nlohmann::json doc = nlohmann::json::parse(serialised);
      doc.at("result_code").get_to(res->error_code);
      if (res->error_code == 0) {
        doc.at("bootstrap_time").get_to(res->bootstrap_time);
        // TODO(bassosimone): understand whether we need to map these fields.
        //doc.at("http_proxy_port").get_to(res->http_proxy_port);
        //doc.at("socks_proxy_port").get_to(res->socks_proxy_port);
      } else {
        doc.at("error").get_to(res->error_reason);
      }
    } catch (const std::exception &) {
      res->error_code = INT64_MAX;  // Make sure we return failure
      res->error_reason = "Failed to parse the returned JSON";
      return res.release();
    }
  }

  // All good! Success! Happy! (Well, maybe it still didn't bootstrap...)
  return res.release();
}

void mkpsiphon_settings_delete(mkpsiphon_settings_t *settings) {
  delete settings;
}

int64_t mkpsiphon_results_get_error_code(const mkpsiphon_results_t *results) {
  if (results == nullptr) {
    MKPSIPHON_ABORT();
  }
  return results->error_code;
}

const char *mkpsiphon_results_get_error_reason(
    const mkpsiphon_results_t *results) {
  if (results == nullptr) {
    MKPSIPHON_ABORT();
  }
  return results->error_reason.c_str();
}

double mkpsiphon_results_get_bootstrap_time(
    const mkpsiphon_results_t *results) {
  if (results == nullptr) {
    MKPSIPHON_ABORT();
  }
  return results->bootstrap_time;
}

void mkpsiphon_results_delete(mkpsiphon_results_t *results) {
  delete results;
}

#endif  // MKPSIPHON_INLINE_IMPL
#endif  // __cplusplus
#endif  // MEASUREMENT_KIT_MKPSIPHON_H
