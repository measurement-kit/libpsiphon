#ifndef MEASUREMENT_KIT_LIBPSIPHON_HPP
#define MEASUREMENT_KIT_LIBPSIPHON_HPP

/// \file psiphon.hpp
///
/// \brief Public header of measurement-kit/libpsiphon.
///
/// Usage example follows.
///
/// ```
/// #include "psiphon.hpp"
///
/// mk::libpsiphon::Client client;
/// Settings settings;
/// settings.timeout = <timeout in seconds>;
/// settings.platform = <client platform string following format specified in libpsiphontunnel.h>;
/// settings.network_id = <network id string following format specified in libpsiphontunnel.h>;
/// Client client = Client(settings);
///
/// std::string config = <psiphon config JSON string>;
///
/// Result res = client.run(config);
/// ```

#include "json.hpp"

#include <string>

using nlohmann::json;

namespace mk {
namespace psiphon {

// Versioning
// ``````````

/// Type containing a version number.
using Version = unsigned int;

/// Major API version number of measurement-kit/libpsiphon.
constexpr Version version_major = Version{0};

/// Minor API version number of measurement-kit/libpsiphon.
constexpr Version version_minor = Version{1};

/// Patch API version number of measurement-kit/libpsiphon.
constexpr Version version_patch = Version{0};

// Settings
// ````````

/// Psiphon client settings.
/// See comments in libpsiphontunnel.h.
class Settings {
public:
  /// Timeout in seconds
  long long timeout;

  /// Client platform string
  /// Must follow format specified in libpsiphontunnel.h
  std::string platform;

  /// Client network id
  /// Must follow format specified in libpsiphontunnel.h
  std::string network_id;
};

// Result
// ``````

/// Psiphon bootstraping connection result.
/// See comments in libpsiphontunnel.h.
struct Result {
  int code; // Psiphon failed to bootstrap if non-zero
  float bootstrap_time; // only set if code == 0
  int http_proxy_port; // only set if code == 0
  int socks_proxy_port; // only set if code == 0
  std::string error; // only set if code != 0
};

void to_json(json& j, const Result& r);
void from_json(const json& j, Result& r);

// Client
// ``````

/// Psiphon client. The most correct way to use this class is to construct a Client,
/// with custom settings providing a desired timeout in seconds, a platform string
/// and a network ID. Then the run() should be called with a valid Psiphon config
/// string. stop() should be called when done to ensure that all resources are
/// cleaned up.
class Client {
 public:
  /// Constructs a Client with default settings.
  Client() noexcept;

  /// Constructs a Client with provided settings.
  Client(Settings settings) noexcept;

  /// Destroys a Client.
  virtual ~Client() noexcept;

  /// Runs a Psiphon test using the configured (or default) settings.
  /// Blocks until Psiphon either connects successfully or times out.
  Result run(std::string) noexcept;

  /// Stops a running client. Blocking.
  void stop() noexcept;

  /// Called when a warning message is emitted. The default behavior is to write
  /// the warning onto the `std::clog` standard stream.
  virtual void on_warning(const std::string &s) noexcept;

  /// Called when an informational message is emitted. The default behavior is
  /// to write the warning onto the `std::clog` standard stream.
  virtual void on_info(const std::string &s) noexcept;

  /// Called when a debug message is emitted. The default behavior is
  /// to write the warning onto the `std::clog` standard stream.
  virtual void on_debug(const std::string &s) noexcept;

 private:
  Settings settings;
};

}  // namespace psiphon
}  // namespace mk

#endif // MEASUREMENT_KIT_LIBPSIPHON_HPP