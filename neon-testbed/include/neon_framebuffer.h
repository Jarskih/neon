// neon_framebuffer.h

#ifndef NEON_FRAMEBUFFER_H_INCLUDED
#define NEON_FRAMEBUFFER_H_INCLUDED

#include "neon_graphics.h"

namespace neon {
   constexpr uint32 MAX_FRAMEBUFFER_ATTACHMENTS = 4;

   enum framebuffer_format {
      FRAMEBUFFER_FORMAT_NONE,
      FRAMEBUFFER_FORMAT_RGB8,
      FRAMEBUFFER_FORMAT_RGBA8,
      FRAMEBUFFER_FORMAT_D32,
      FRAMEBUFFER_FORMAT_COUNT,
      FRAMEBUFFER_FORMAT_INVALID,
   };

   struct framebuffer {
      static void unbind(int32 width, int32 height);

      framebuffer();

      bool is_valid() const;
      bool create(int32 width, int32 height, 
                  int32 color_attachment_format_count,
                  const framebuffer_format *color_attachment_formats,
                  framebuffer_format depth_attachment_format = FRAMEBUFFER_FORMAT_NONE);
      void destroy();

      void bind();
      void blit(int32 x, int32 y, int32 width, int32 height);

      int32 width_;
      int32 height_;
      uint32 id_;
      uint32 depth_attachment_;
      uint32 color_attachments_[MAX_FRAMEBUFFER_ATTACHMENTS];
   };
} // !neon

#endif // !NEON_FRAMEBUFFER_H_INCLUDED
