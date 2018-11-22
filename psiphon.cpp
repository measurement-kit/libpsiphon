#include "psiphon.hpp"
#include "json.hpp"
#include "vendor/libpsiphontunnel.h"

#include <iostream>

namespace mk {
namespace psiphon {

void to_json(json& j, const Result& r) {
  j = json{{"result_code", r.code}, 
           {"bootstrap_time", r.bootstrap_time},
           {"http_proxy_port", r.http_proxy_port},
           {"socks_proxy_port", r.socks_proxy_port},
           {"error", r.error}};
}

void from_json(const json& j, Result& r) {
  j.at("result_code").get_to(r.code);

  if (r.code == 0) {
    // If result_code == 0 these fields will be present
    // See comments in libpsiphontunnel.h
    j.at("bootstrap_time").get_to(r.bootstrap_time);
    j.at("http_proxy_port").get_to(r.http_proxy_port);
    j.at("socks_proxy_port").get_to(r.socks_proxy_port);
  } else {
    // If result_code != 0 these fields will be present
    // See comments in libpsiphontunnel.h
    j.at("error").get_to(r.error);
  }
}

// Top-level API

Client::Client() noexcept {}

Client::Client(Settings settings) noexcept : Client::Client() {
  this->settings = settings;
}

Client::~Client() noexcept {}

Result Client::run(std::string config) noexcept {
  GoString configJSON;
  configJSON.p = config.c_str();
  configJSON.n = config.size();

  GoString embeddedServerList = {};

  GoString client_platform = {settings.platform.c_str(), (long)settings.platform.length()};
  GoString network_id = {settings.network_id.c_str(), (long)settings.network_id.length()};

  auto tmp = Start(configJSON, embeddedServerList, client_platform, network_id, settings.timeout);

  std::string result(tmp);

  json j = json::parse(result);
  Result r = j;

  return r;
}

void Client::stop() noexcept { 
  Stop();
}

void Client::on_warning(const std::string &msg) noexcept {
  std::clog << "[!] " << msg << std::endl;
}

void Client::on_info(const std::string &msg) noexcept {
  std::clog << msg << std::endl;
}

void Client::on_debug(const std::string &msg) noexcept {
  std::clog << "[D] " << msg << std::endl;
}

}  // namespace psiphon
}  // namespace mk
