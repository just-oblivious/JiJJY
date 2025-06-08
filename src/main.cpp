/*
Inspired by:
 - txtempus radio time project: https://github.com/hzeller/txtempus
 - ESP8266 NTP Setup: https://werner.rothschopf.net/202011_arduino_esp8266_ntp_en.htm
 - JJY spec: https://www.nict.go.jp/en/sts/jjy_signal.html
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <coredecls.h>


#include "config.h"
#include "logging.h"
#include "transmit.h"
#include "jjy.h"


bool timeSynced = false;
uint32_t lastTimeSync = 0;
uint32_t currentMillis = 0;
uint8_t currentSecond = 0;


void timeSyncCb() {
  lastTimeSync = millis();
  timeSynced = true;
  log("NTP:SYNC");
}

bool elapsedSince(uint32_t since, uint32_t ms) {
  return (currentMillis - since) >= ms;
}

void setup() {
  // Configure IO
  pinMode(BUILTIN_LED, OUTPUT);
  setupTransmitPin(TRANSMIT_PIN);
  digitalWrite(BUILTIN_LED, HIGH);

  // Configure UDP logger
  setupLogger(UDP_HOST, UDP_PORT, 54321);

  // Configure NTP server
  configTime(TZ, NTP_SERVER);
  settimeofday_cb(timeSyncCb);

  // Configure WiFi connection
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  bool ledState = false;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    digitalWrite(BUILTIN_LED, ledState);
    ledState = !ledState;
  }
  digitalWrite(BUILTIN_LED, HIGH);

  log("STATUS:ONLINE");

  while(!timeSynced) {}  // Wait for initial NTP sync

  log("STATUS:RUN");
}

// tick is called at the beginning of every second
void tick(struct tm *tm) {
  // If the time is not NTP-synced, pause transmission
  if (!timeSynced) {
    stopTx();
    log("NTP:NOSYNC");
    return;
  }

  // At second 0, update the packet data and log the current time
  if (tm->tm_sec == 0) {
    prepareMinute(tm);
    char isoTime[32];
    strftime(isoTime, sizeof(isoTime), "TIME:%Y-%m-%dT%H:%M:%S", tm);
    log(isoTime);
  }

  // Transmit the bit value for the current second
  uint32_t mod = getModulationForSecond(tm->tm_sec);
  flashTx(mod);

  // Alternate the status LED to indicate that bits are being transmitted
  if (mod != 0) {
    digitalWrite(BUILTIN_LED, tm->tm_sec % 2);
  }
}

void loop() {
  currentMillis = millis();

  // Get the current time
  time_t now;
  struct tm tm;

  time(&now);
  localtime_r(&now, &tm);

  // Consider the time to be out of sync if the last successful NTP sync was > 3 hours ago
  if (elapsedSince(lastTimeSync, 3600 * 1000 * 3)) {
    timeSynced = false;
  }

  // Call tick() when the second ticks over
  if (currentSecond != tm.tm_sec) {
    tick(&tm);
  }
  currentSecond = tm.tm_sec;
}
