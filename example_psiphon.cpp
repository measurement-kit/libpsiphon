#include "psiphon.hpp"

#include <iostream>
#include <fstream>
#include <string>

using namespace mk::psiphon;

int main() {

  Settings settings;

  // Set timeout (in seconds)
  settings.timeout = 10;

  // set client platform
  settings.platform = "OSName_OSVersion_com.example.libpsiphon";

  // set network ID
  settings.network_id = "TEST";

  Client client = Client(settings);

  std::ifstream t("psiphon-config.json");
  std::string config((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

  std::clog << "Starting Psiphon..." << std::endl;
  Result res = client.run(config);
  if (res.code == 0) {
    std::clog << "Psiphon connected in " << res.bootstrap_time << " seconds" << std::endl;
    std::clog << "Http proxy running on port: " << res.http_proxy_port << std::endl;
    std::clog << "Socks proxy running on port: " << res.socks_proxy_port << std::endl;
  } else {
    std::clog << "Psiphon failed to connect with error: " << res.error << std::endl;
  }

  std::clog << "Stopping Psiphon..." << std::endl;
  client.stop();
  std::clog << "Psiphon stopped" << std::endl;
  std::clog << "Done." << std::endl;
}
