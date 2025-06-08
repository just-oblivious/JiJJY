#include "transmit.h"

uint8_t _txPin = 0;
Ticker _txTick;

void setupTransmitPin(uint8_t pin) {
  analogWriteFreq(40000);  // 40 kHz PWM freq to emulate the Ōtakadoyayama JJY Transmitter
  pinMode(pin, OUTPUT);
  _txPin = pin;
}

// output a square wave
void IRAM_ATTR beginTx() {
  analogWrite(_txPin, 128);  // 50% PWM duty cycle
}

// stop transmitting
void IRAM_ATTR stopTx() {
  analogWrite(_txPin, 0);
}

// transmit for ms milliseconds
void IRAM_ATTR flashTx( uint32_t ms) {
  beginTx();
  _txTick.once_ms(ms, stopTx);
}
