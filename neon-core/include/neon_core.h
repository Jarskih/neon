// neon_core.h

#ifndef NEON_CORE_H_INCLUDED
#define NEON_CORE_H_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>

namespace neon {
   typedef unsigned long long uint64;
   typedef   signed long long int64;
   typedef unsigned int       uint32;
   typedef   signed int       int32;
   typedef unsigned short     uint16;
   typedef   signed short     int16;
   typedef unsigned char      uint8;
   typedef   signed char      int8;

   using string = std::string;

   template <typename T>
   using dynamic_array = std::vector<T>;

   template <typename K, typename V>
   using hashmap = std::unordered_map<K, V>;

   struct math {
      static inline float sqrt(const float value);
      static inline float to_radians(const float degrees);
      static inline float to_degrees(const float radians);
      static inline float cos(const float radians);
      static inline float sin(const float radians);
   };

   struct time {
      static time now();

      time();
      explicit time(int64 tick);

      time operator+(const time &rhs) const;
      time operator-(const time &rhs) const;
      time &operator+=(const time &rhs);
      time &operator-=(const time &rhs);

      bool operator<(const time &rhs) const;
      bool operator>(const time &rhs) const;
      bool operator==(const time &rhs) const;
      bool operator!=(const time &rhs) const;

      float as_seconds() const;
      float as_milliseconds() const;

      int64 tick_;
   };

   struct file_system {
      static bool exists(const string &filename);
      static bool read_file_content(const string &filename, dynamic_array<uint8> &content);
      static bool write_file_content(const string &filename, const dynamic_array<uint8> &content, bool allow_overwrite);
      static bool remove_file(const string &filename);
      static bool create_directory(const string &name);
      static string get_app_directory();
      static string get_save_directory(const string &app_name);

      file_system();
   };

   enum keycode {
      KEYCODE_NONE = 0x00,        KEYCODE_BACK = 0x08,        KEYCODE_TAB = 0x09,         KEYCODE_CLEAR = 0x0C,
      KEYCODE_RETURN = 0x0D,      KEYCODE_SHIFT = 0x10,       KEYCODE_CONTROL = 0x11,     KEYCODE_MENU = 0x12,
      KEYCODE_PAUSE = 0x13,       KEYCODE_CAPITAL = 0x14,     KEYCODE_ESCAPE = 0x1B,      KEYCODE_SPACE = 0x20,
      KEYCODE_PAGEUP = 0x21,      KEYCODE_PAGEDOWN = 0x22,    KEYCODE_END = 0x23,         KEYCODE_HOME = 0x24,
      KEYCODE_LEFT = 0x25,        KEYCODE_UP = 0x26,          KEYCODE_RIGHT = 0x27,       KEYCODE_DOWN = 0x28,
      KEYCODE_KEYPADENTER = 0x2B, KEYCODE_PRINTSCREEN = 0x2C, KEYCODE_INSERT = 0x2D,      KEYCODE_DELETE = 0x2E,
      KEYCODE_0 = 0x30,           KEYCODE_1 = 0x31,           KEYCODE_2 = 0x32,           KEYCODE_3 = 0x33,
      KEYCODE_4 = 0x34,           KEYCODE_5 = 0x35,           KEYCODE_6 = 0x36,           KEYCODE_7 = 0x37,
      KEYCODE_8 = 0x38,           KEYCODE_9 = 0x39,           KEYCODE_A = 0x41,           KEYCODE_B = 0x42,
      KEYCODE_C = 0x43,           KEYCODE_D = 0x44,           KEYCODE_E = 0x45,           KEYCODE_F = 0x46,
      KEYCODE_G = 0x47,           KEYCODE_H = 0x48,           KEYCODE_I = 0x49,           KEYCODE_J = 0x4A,
      KEYCODE_K = 0x4B,           KEYCODE_L = 0x4C,           KEYCODE_M = 0x4D,           KEYCODE_N = 0x4E,
      KEYCODE_O = 0x4F,           KEYCODE_P = 0x50,           KEYCODE_Q = 0x51,           KEYCODE_R = 0x52,
      KEYCODE_S = 0x53,           KEYCODE_T = 0x54,           KEYCODE_U = 0x55,           KEYCODE_V = 0x56,
      KEYCODE_W = 0x57,           KEYCODE_X = 0x58,           KEYCODE_Y = 0x59,           KEYCODE_Z = 0x5A,
      KEYCODE_LWIN = 0x5B,        KEYCODE_RWIN = 0x5C,        KEYCODE_KEYPAD0 = 0x60,     KEYCODE_KEYPAD1 = 0x61,
      KEYCODE_KEYPAD2 = 0x62,     KEYCODE_KEYPAD3 = 0x63,     KEYCODE_KEYPAD4 = 0x64,     KEYCODE_KEYPAD5 = 0x65,
      KEYCODE_KEYPAD6 = 0x66,     KEYCODE_KEYPAD7 = 0x67,     KEYCODE_KEYPAD8 = 0x68,     KEYCODE_KEYPAD9 = 0x69,
      KEYCODE_MULTIPLY = 0x6A,    KEYCODE_ADD = 0x6B,         KEYCODE_SEPARATOR = 0x6C,   KEYCODE_SUBTRACT = 0x6D,
      KEYCODE_DECIMAL = 0x6E,     KEYCODE_DIVIDE = 0x6F,      KEYCODE_F1 = 0x70,          KEYCODE_F2 = 0x71,
      KEYCODE_F3 = 0x72,          KEYCODE_F4 = 0x73,          KEYCODE_F5 = 0x74,          KEYCODE_F6 = 0x75,
      KEYCODE_F7 = 0x76,          KEYCODE_F8 = 0x77,          KEYCODE_F9 = 0x78,          KEYCODE_F10 = 0x79,
      KEYCODE_F11 = 0x7A,         KEYCODE_F12 = 0x7B,         KEYCODE_NUMLOCK = 0x90,     KEYCODE_SCROLLOCK = 0x91,
      KEYCODE_LSHIFT = 0xA0,      KEYCODE_RSHIFT = 0xA1,      KEYCODE_LCONTROL = 0xA2,    KEYCODE_RCONTROL = 0xA3,
      KEYCODE_LMENU = 0xA4,       KEYCODE_RMENU = 0xA5,       KEYCODE_OEM_1 = 0xBA,       KEYCODE_OEM_PLUS = 0xBB,
      KEYCODE_OEM_COMMA = 0xBC,   KEYCODE_OEM_MINUS = 0xBD,   KEYCODE_OEM_PERIOD = 0xBE,  KEYCODE_OEM_2 = 0xBF, 
      KEYCODE_OEM_3 = 0xC0,       KEYCODE_OEM_4 = 0xDB,       KEYCODE_OEM_5 = 0xDC,       KEYCODE_OEM_6 = 0xDD, 
      KEYCODE_OEM_7 = 0xDE,       KEYCODE_OEM_8 = 0xDF,       KEYCODE_OEM_102 = 0xE2,     KEYCODE_COUNT = 256,
   };

   struct keyboard {
      keyboard();

      bool is_down(keycode index) const;
      bool is_pressed(keycode index) const;
      bool is_released(keycode index) const;

      struct keystate {
         bool down_;
         bool released_;
         bool pressed_;
      } keys_[KEYCODE_COUNT];
   };

   enum mouse_button {
      MOUSE_BUTTON_LEFT,
      MOUSE_BUTTON_RIGHT,
      MOUSE_BUTTON_MIDDLE,
      MOUSE_BUTTON_COUNT,
   };

   struct mouse {
      mouse();

      bool is_down(mouse_button index) const;
      bool is_pressed(mouse_button index) const;
      bool is_released(mouse_button index) const;

      int32 x_;
      int32 y_;
      int32 delta_;
      struct buttonstate {
         bool down_;
         bool released_;
         bool pressed_;
      } buttons_[MOUSE_BUTTON_COUNT];
   };

   struct window {
      struct display_mode {
         display_mode();
         explicit display_mode(int32 width, int32 height);

         int32 width_;
         int32 height_;
      };

      static bool get_available_display_modes(dynamic_array<display_mode> &modes);
      static bool get_desktop_display_mode(display_mode &mode);
      static bool get_display_mode(display_mode &mode);
      static void set_display_mode(const display_mode &mode);
   };

   struct application {
      static application *create(int &width, int &height, string &title);

      application();
      virtual ~application() = default;

      bool init();
      void shut();
      bool frame();

      virtual bool enter() = 0;
      virtual void exit() = 0;
      virtual bool tick(const time &dt) = 0;

      keyboard keyboard_;
      mouse mouse_;
      time start_;
      time current_;
   };

   struct image {
      enum format {
         INVALID,
         RGBA8,
         COUNT,
      };

      image();

      bool is_valid() const;
      bool create_from_file(const char *filename);
      bool create_from_memory(const int32 width, const int32 height, uint8 *data);
      void destroy();

      format pixel_format() const;
      int32 width() const;
      int32 height() const;
      uint8 *data() const;

      format format_;
      int32 width_;
      int32 height_;
      uint8 *data_;
   };
} // !neon

#endif //!NEON_CORE_H_INCLUDED
