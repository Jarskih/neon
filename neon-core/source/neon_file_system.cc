// neon_file_system.cc

#include "neon_core.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlobj.h> // SHGetKnownFolderPath

namespace neon {
   template <typename Fn>
   struct scope_guard {
      scope_guard(Fn f) : fn(f) { }
      ~scope_guard() { fn(); }
      Fn fn;
   };

   template <typename Fn>
   scope_guard<Fn> make_scope_guard(Fn f) {
      return scope_guard<Fn>(f);
   }

   namespace {
      void string_replace(std::string &str, char old_ch, char new_ch) {
         std::string::size_type pos;
         while ((pos = str.find(old_ch)) != std::string::npos) {
            str.replace(pos, 1, 1, new_ch);
         }
      }

      bool utf8_encode(const std::wstring &wstr, std::string &str) {
         if (wstr.empty()) {
            return false;
         }

         int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);

         str.resize(size_needed, 0);
         WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), (LPSTR)str.data(), size_needed, NULL, NULL);

         return true;
      }
   } // !anon

   // static 
   bool file_system::exists(const string &filename) {
      DWORD attrib = GetFileAttributesA(filename.c_str());
      return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
   }

   bool file_system::read_file_content(const string &filename, dynamic_array<uint8> &content) {
      HANDLE handle = CreateFileA(filename.c_str(),
                                  GENERIC_READ,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
      if (handle == INVALID_HANDLE_VALUE) {
         return false;
      }

      auto defer = make_scope_guard(([&]() {
         CloseHandle(handle);
      }));

      LARGE_INTEGER size = {};
      if (!GetFileSizeEx(handle, &size)) {
         return false;
      }

      content.resize(size.QuadPart);
      if (!ReadFile(handle, content.data(), size.LowPart, NULL, NULL)) {
         return false;
      }

      if (size.HighPart > 0) {
         if (!ReadFile(handle, content.data() + size.LowPart, size.HighPart, NULL, NULL)) {
            return false;
         }
      }

      return true;
   }

   bool file_system::write_file_content(const string &filename, const dynamic_array<uint8> &content, bool allow_overwrite) {
      HANDLE handle = CreateFileA(filename.c_str(),
                                  GENERIC_WRITE,
                                  FILE_SHARE_WRITE,
                                  NULL,
                                  allow_overwrite ? CREATE_ALWAYS : CREATE_NEW,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
      if (handle == INVALID_HANDLE_VALUE) {
         return false;
      }

      auto defer = make_scope_guard(([&]() {
         CloseHandle(handle);
      }));

      if (allow_overwrite) {
         DWORD error = GetLastError();
         if (error != ERROR_ALREADY_EXISTS) {
            return false;
         }
      }

      LARGE_INTEGER size = {};
      size.QuadPart = content.size();
      if (!WriteFile(handle, content.data(), size.LowPart, NULL, NULL)) {
         return false;
      }

      if (size.HighPart > 0) {
         if (!WriteFile(handle, content.data() + size.LowPart, size.HighPart, NULL, NULL)) {
            return false;
         }
      }

      return true;
   }

   bool file_system::remove_file(const string &filename) {
      if (!DeleteFileA(filename.c_str())) {
         DWORD error = GetLastError();
         switch (error) {
            case ERROR_FILE_NOT_FOUND:
               //case ERROR_ACCESS_DENIED:
               break;
            default:
               return false;
         }
      }

      return true;
   }

   bool file_system::create_directory(const string &name) {
      HRESULT hr = SHCreateDirectoryExA(NULL, name.c_str(), NULL);
      if (FAILED(hr)) {
         switch (hr) {
            case ERROR_FILE_EXISTS:
            case ERROR_ALREADY_EXISTS:
               break;
            default:
               return false;
         }
      }

      return true;
   }

   string file_system::get_app_directory() {
      string result;
      char buf[MAX_PATH] = {};
      if (GetModuleFileNameA(GetModuleHandle(NULL), buf, MAX_PATH) != 0) {
         string_replace(result, '\\', '/');
         result = result.substr(0, result.find_last_of('/'));
         result += '/';
      }

      return result;
   }

   string file_system::get_save_directory(const string &app_name) {
      string result;
      WCHAR *ppszPath = nullptr;
      HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &ppszPath);
      if (SUCCEEDED(hr)) {
         std::wstring path(ppszPath);
         CoTaskMemFree(ppszPath);

         hr = SHCreateDirectory(NULL, path.c_str());
         if (hr != S_OK) {
            switch (hr) {
               case ERROR_FILE_EXISTS:
               case ERROR_ALREADY_EXISTS:
                  break;
               default:
                  return "";
            }
         }

         if (!utf8_encode(path, result)) {
            return "";
         }

         string_replace(result, '\\', '/');
         result.append(app_name);
         result += '/';
      }

      return result;
   }

   file_system::file_system()
   {
   }
} // !neon
