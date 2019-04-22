#ifndef RYHTHMMATH_H
#define RYHTHMMATH_H

#include <functional>
#include <cmath>
#include <cassert>

namespace RhythmEngine
{
  template<typename T>
  inline constexpr T GetPrevIntervalTime(const T& currentTime, const T& intervalTime)
  {
    assert(currentTime >= 0 && intervalTime != 0);
    return intervalTime * std::floor(currentTime / intervalTime);
  }

  // Notification : Return value of GetNextIntervalTime(0) is zero! This is for optimization.
  template<typename T>
  inline constexpr T GetNextIntervalTime(const T& currentTime, const T& intervalTime)
  {
    assert(currentTime > 0.0);
    assert(intervalTime != 0.0);
    return intervalTime * std::ceil(currentTime / intervalTime); // More Fast
    // return (currentTime == 0) ? intervalTime : intervalTime * std::ceil(currentTime / intervalTime);
  }
}

#endif