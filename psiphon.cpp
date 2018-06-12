#include "psiphon.hpp"
#include "vendor/psi.h"

#include <iostream>

namespace mk {
namespace psiphon {

// Top-level API

Client::Client() noexcept {};

Client::Client(Settings settings) noexcept : Client::Client() {
  settings = settings;
}

Client::~Client() noexcept {}

bool Client::run(std::string config) noexcept {
  GoString embeddedServerList;
  embeddedServerList.p = "";
  embeddedServerList.n = 0;

  std::clog << config << std::endl;

  GoString configJSON;
  configJSON.p = config.c_str();
  configJSON.n = config.size();

  auto tmp = Start(configJSON, embeddedServerList);
  std::string result(tmp);
  std::clog << result << std::endl;
  return true;
}

bool Client::stop() noexcept { return true; }

void Client::on_warning(const std::string &msg) noexcept {
  std::clog << "[!] " << msg << std::endl;
}

void Client::on_info(const std::string &msg) noexcept {
  std::clog << msg << std::endl;
}

void Client::on_debug(const std::string &msg) noexcept {
  std::clog << "[D] " << msg << std::endl;
}

void Client::on_connect(std::string proxy_info,
                        double bootstrap_time) noexcept {
  std::clog << "proxy_info: " << proxy_info
            << " bootstrap_time: " << bootstrap_time << std::endl;
}

}  // namespace psiphon
}  // namespace mk
