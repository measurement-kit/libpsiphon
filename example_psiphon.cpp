#include <fstream>
#include <string>
#include "psiphon.hpp"

using namespace mk::psiphon;

int main() {
  Client client;
  std::ifstream t("psiphon-config.json");
  std::string config((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

  // XXX send this to another thread.
  client.run(config);
  client.stop();
}
