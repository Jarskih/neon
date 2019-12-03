// neon_application.cc

#include "neon_core.h"

namespace neon {
   application::application()
   {
   }

   bool application::init() {
      start_ = time::now();
      current_ = start_;

      if (!enter()) {
         return false;
      }

      return true;
   }

   void application::shut() {
      exit();
   }

   bool application::frame() {
      const time now = time::now();
      time deltatime_ = now - current_;
      current_ = now;

      if (!tick(deltatime_)) {
         return false;
      }

      return true;
   }
} // !neon
