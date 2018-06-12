#pragma once

#include <string>

namespace mk {
namespace psiphon {

class Settings {
  std::string sponsor_id;
  std::string propagation_id;
  std::string platform;
  std::string session_id;
};

class Client {
 public:
  Client() noexcept;

  Client(Settings settings) noexcept;

  virtual ~Client() noexcept;

  bool run() noexcept;

  bool stop() noexcept;

  /// Called when a warning message is emitted. The default behavior is to write
  /// the warning onto the `std::clog` standard stream.
  virtual void on_warning(const std::string &s) noexcept;

  /// Called when an informational message is emitted. The default behavior is
  /// to write the warning onto the `std::clog` standard stream.
  virtual void on_info(const std::string &s) noexcept;

  /// Called when a debug message is emitted. The default behavior is
  /// to write the warning onto the `std::clog` standard stream.
  virtual void on_debug(const std::string &s) noexcept;

  // on_connect returns a JSON serialized proxy endpoint configuration
  virtual void on_connect(std::string proxy_info,
                          double bootstrap_time) noexcept;

 private:
  Settings settings;
};

}  // namespace psiphon
}  // namespace mk
