#include "logging.h"

WiFiUDP udp;

uint16_t _dstPort = 1337;
const char *_dstHost = "255.255.255.255";

void setupLogger(const char *dstHost, uint16_t dstPort, uint16_t srcPort) {
  _dstHost = dstHost;
  _dstPort = dstPort;
  udp.begin(srcPort);
}

void log(const char *message) {
  udp.beginPacket(_dstHost, _dstPort);
  udp.print(message);
  udp.print("\n");
  udp.endPacket();
}
