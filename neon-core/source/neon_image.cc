// neon_image.cc

#include "neon_core.h"

#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace neon {
   image::image()
      : format_(INVALID)
      , width_(0)
      , height_(0)
      , data_(nullptr)
   {
   }

   bool image::is_valid() const {
      return data_ != nullptr;
   }

   bool image::create_from_file(const char *filename) {
      destroy();

      dynamic_array<uint8> file_content;
      if (!file_system::read_file_content(filename, file_content)) {
         return false;
      }

      int width = 0, height = 0, comp = 0;
      auto data = stbi_load_from_memory(file_content.data(), (int)file_content.size(),
                                        &width, &height, &comp, STBI_rgb_alpha);
      if (!data) {
         return false;
      }

      width_ = width;
      height_ = height;
      data_ = data;

      return true;
   }

   bool image::create_from_memory(const int32 width, const int32 height, uint8 *data) {
      destroy();

      width_ = width;
      height_ = height;
      data_ = (uint8 *)malloc(width_ * height_ * 4);
      if (!data_) {
         return false;
      }
      
      memcpy(data_, data, width_ * height_ * 4);

      return true;
   }

   void image::destroy() {
      if (is_valid()) {
         free(data_);

         width_ = 0;
         height_ = 0;
         data_ = nullptr;
      }
   }

   image::format image::pixel_format() const {
      return format_;
   }

   int32 image::width() const {
      return width_;
   }

   int32 image::height() const {
      return height_;
   }

   uint8 *image::data() const {
      return data_;
   }
} // !neon
