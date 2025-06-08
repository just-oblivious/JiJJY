#pragma once

#include <coredecls.h>
#include <WiFiUdp.h>

void setupLogger(const char *dstHost, uint16_t dstPort, uint16_t srcPort);
void log(const char *message);
