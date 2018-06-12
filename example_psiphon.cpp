#include "psiphon.hpp"

using namespace mk::psiphon;

int main() {
  Client client;

  // XXX send this to another thread.
  client.run();
  client.stop();
}
