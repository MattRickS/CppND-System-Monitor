#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

const int SECONDS_IN_DAY = 3600 * 24;

std::string Format::ElapsedTime(long seconds) {
  seconds %= SECONDS_IN_DAY;
  std::stringstream s;
  s << std::setfill('0') << std::setw(2) << seconds / 3600 << ":"
    << std::setw(2) << (seconds % 3600) / 60 << ":" << std::setw(2)
    << seconds % 60;
  return s.str();
}
