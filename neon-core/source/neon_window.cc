// neon_window.cc

#include "neon_core.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

extern HWND win32_get_window_handle();

namespace neon {
   window::display_mode::display_mode()
      : width_(0)
      , height_(0)
   {
   }

   window::display_mode::display_mode(int32 width, int32 height)
      : width_(width)
      , height_(height)
   {
   }

   // static
   bool window::get_available_display_modes(dynamic_array<display_mode> &modes) {
      DEVMODE desktop_mode = {};
      EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktop_mode);
      const DWORD bits_per_pixel = desktop_mode.dmBitsPerPel;

      for (int index = 0; ; index++) {
         DEVMODE mode = {};
         if (!EnumDisplaySettings(NULL, index, &mode)) {
            break;
         }

         if (mode.dmBitsPerPel != bits_per_pixel) {
            continue;
         }

         if (mode.dmPelsWidth < 1024) {
            continue;
         }

         if (mode.dmPelsHeight > mode.dmPelsWidth) {
            continue;
         }

         modes.push_back(display_mode((int)mode.dmPelsWidth, (int)mode.dmPelsHeight));
      }

      return !modes.empty();
   }

   bool window::get_desktop_display_mode(display_mode &mode) {
      DEVMODE dm = {};
      EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
      mode = display_mode((int)dm.dmPelsWidth, (int)dm.dmPelsHeight);
      return true;
   }

   void window::set_display_mode(const display_mode &mode) {
      HWND window = win32_get_window_handle();
      RECT window_rect = { 0 };
      GetWindowRect(window, &window_rect);

      int window_x = window_rect.left;
      int window_y = window_rect.top;

      window_rect = {};
      window_rect.right = mode.width_;
      window_rect.bottom = mode.height_;
      DWORD style = GetWindowStyle(window);
      AdjustWindowRect(&window_rect, style, 0);

      int window_width = window_rect.right - window_rect.left;
      int window_height = window_rect.bottom - window_rect.top;
      SetWindowPos(window,
                   NULL,
                   window_x,
                   window_y,
                   window_width,
                   window_height,
                   SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
   }

   bool window::get_display_mode(display_mode &mode) {
      HWND window = win32_get_window_handle();
      RECT client_rect = { 0 };
      if (GetClientRect(window, &client_rect) == TRUE) {
         mode.width_ = (client_rect.right - client_rect.left);
         mode.height_ = (client_rect.bottom - client_rect.top);
         return true;
      }
      return false;
   }
} // !neon
