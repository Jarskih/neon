// neon_math.cc

#include "neon_core.h"

#include <cmath>

namespace neon {
   constexpr float PI = 3.141592f;

   inline float math::sqrt(const float value) {
      return sqrtf(value);
   }

   inline float math::to_radians(const float degrees) {
      return degrees * PI / 180.0f;
   }

   inline float math::to_degrees(const float radians) {
      return radians * 180.0f / PI;
   }

   inline float math::cos(const float radians) {
      return cosf(radians);
   }

   inline float math::sin(const float radians) {
      return sinf(radians);
   }
} // !neon
