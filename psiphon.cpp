#include "psiphon.hpp"

#include <iostream>

namespace mk {
namespace psiphon {

// Top-level API

Client::Client() noexcept {};

Client::Client(Settings settings) noexcept : Client::Client() {
  settings = settings;
}

Client::~Client() noexcept {}

bool Client::run() noexcept { return true; }

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
