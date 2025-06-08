#pragma once

#include <coredecls.h>
#include <Ticker.h>

void setupTransmitPin(uint8_t pin);
void beginTx();
void stopTx();
void flashTx(uint32_t ms);
