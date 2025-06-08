/* Configuration options for ESP-01 JJY time signal transmitter */
#pragma once

/* NTP config */
#define NTP_SERVER "nl.pool.ntp.org"
#define TZ "CET-1CEST,M3.5.0,M10.5.0/3" // tzdb entry for Europe/Amsterdam (https://data.iana.org/time-zones/tz-link.html#tzdb)

/* WiFi Config */
#define WIFI_SSID "Wifi SSID"
#define WIFI_PSK "Wifi Password"

/* Status messages endpoint */
#define UDP_HOST "255.255.255.255"  // Broadcast status messages
#define UDP_PORT 1337

/* Pin definitions */
#define TRANSMIT_PIN 3
#define BUILTIN_LED 1  // BUILTIN_LED can be pin 1 or 2 depending on the board
