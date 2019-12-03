// neon_keyboard.cc

#include "neon_core.h"

namespace neon {
   keyboard::keyboard()
      : keys_{}
   {
   }

   bool keyboard::is_down(keycode index) const {
      return keys_[index].down_;
   }

   bool keyboard::is_pressed(keycode index) const {
      return keys_[index].pressed_;
   }

   bool keyboard::is_released(keycode index) const {
      return keys_[index].released_;
   }
} // !neon
