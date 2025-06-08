#include "jjy.h"

// JJY implementation adopted from: https://github.com/hzeller/txtempus
uint64_t _timeBits = 0;


// Similar to WWVB, JJY uses BCD, but usually has a zero bit between the digits.
// So let's call it 'padded' BCD.
static uint64_t to_padded5_bcd(int n) {
  return (((n / 100) % 10) << 10) | (((n / 10) % 10) << 5) | (n % 10);
}

// Regular BCD for the year encoding.
static uint64_t to_bcd(int n) {
  return (((n / 100) % 10) << 8) | (((n / 10) % 10) << 4) | (n % 10);
}

static uint64_t parity(uint64_t d, uint8_t from, uint8_t to_including) {
  uint8_t result = 0;
  for (int bit = from; bit <= to_including; ++bit) {
    if (d & (1LL << bit)) result++;
  }
  return result & 0x1;
}

void prepareMinute(struct tm *breakdown) {
  // https://en.wikipedia.org/wiki/JJY
  // The JJY format uses Bit-Bigendianess, so we'll start with the first
  // bit left in our integer in bit 59.
  _timeBits = 0;  // All the unused bits are zero.
  _timeBits |= to_padded5_bcd(breakdown->tm_min) << (59 - 8);
  _timeBits |= to_padded5_bcd(breakdown->tm_hour) << (59 - 18);
  _timeBits |= to_padded5_bcd(breakdown->tm_yday + 1) << (59 - 33);
  _timeBits |= to_bcd(breakdown->tm_year % 100) << (59 - 48);
  _timeBits |= to_bcd(breakdown->tm_wday) << (59 - 52);

  _timeBits |= parity(_timeBits, 59-18, 59-12) << (59 - 36);  // PA1
  _timeBits |= parity(_timeBits, 59-8, 59-1) << (59 - 37);    // PA2

  // There is a different 'service announcement' encoding in minute 15 and 45,
  // but let's just ignore that for now. Consumer clocks probably don't care.
}

uint32_t getModulationForSecond(int sec) {
  if (_timeBits == 0) {
    return 0;
  }

  if (sec == 0 || sec % 10 == 9 || sec > 59) {
    return 200;
  }
  const bool bit = _timeBits & (1LL << (59 - sec));
  return bit ? 500 : 800;
}
