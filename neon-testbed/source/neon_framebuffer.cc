// neon_framebuffer.cc

#include "neon_framebuffer.h"

static const GLenum gl_framebuffer_format_internal[] =
{
   GL_NONE,
   GL_RGB,
   GL_RGBA,
   GL_DEPTH24_STENCIL8,
};

static const GLenum gl_framebuffer_format[] =
{
   GL_NONE,
   GL_RGB,
   GL_RGBA,
   GL_DEPTH_STENCIL,
};

static GLenum gl_framebuffer_type[] =
{
   GL_NONE,
   GL_UNSIGNED_BYTE,
   GL_UNSIGNED_BYTE,
   GL_UNSIGNED_INT_24_8,
};

namespace neon {
   namespace {
      void opengl_error_check() {
         GLenum err = glGetError();
         if (err != GL_NO_ERROR) {
            assert(!"opengl error code!");
         }
      }
   } // !anon

   // static 
   void framebuffer::unbind(int32 width, int32 height) {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, width, height);
   }

   framebuffer::framebuffer()
      : width_(0)
      , height_(0)
      , id_(0)
      , depth_attachment_(0)
      , color_attachments_{}
   {
   }

   bool framebuffer::is_valid() const {
      return id_ != 0;
   }

   bool framebuffer::create(int32 width, int32 height,
                            int32 color_attachment_format_count,
                            const framebuffer_format *color_attachment_formats,
                            framebuffer_format depth_attachment_format)
   {
      if (is_valid()) {
         return false;
      }

      GLuint id = 0;
      glGenFramebuffers(1, &id);
      glBindFramebuffer(GL_FRAMEBUFFER, id);

      GLuint textures[MAX_FRAMEBUFFER_ATTACHMENTS] = {};
      glGenTextures(color_attachment_format_count, textures);

      int32 color_attachment_count = 0;
      int32 depth_attachment_count = 0;

      for (int32 attachment_index = 0;
           attachment_index < color_attachment_format_count;
           attachment_index++)
      {
         const framebuffer_format format = color_attachment_formats[attachment_index];

         glBindTexture(GL_TEXTURE_2D, textures[attachment_index]);
         glTexImage2D(GL_TEXTURE_2D,
                      0,
                      gl_framebuffer_format_internal[format],
                      width,
                      height,
                      0,
                      gl_framebuffer_format[format],
                      gl_framebuffer_type[format],
                      nullptr);

         opengl_error_check();

         if (format == FRAMEBUFFER_FORMAT_D32) {
            assert(depth_attachment_count < 1);

            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_TEXTURE_2D,
                                   textures[attachment_index],
                                   0);

            depth_attachment_count++;
         }
         else {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + color_attachment_count,
                                   GL_TEXTURE_2D,
                                   textures[attachment_index],
                                   0);

            color_attachment_count++;
         }

         opengl_error_check();
      }

      GLuint rbo = 0;
      if (depth_attachment_format != FRAMEBUFFER_FORMAT_NONE) {
         glGenRenderbuffers(1, &rbo);
         glBindRenderbuffer(GL_RENDERBUFFER, rbo);

         glRenderbufferStorage(GL_RENDERBUFFER,
                               gl_framebuffer_format_internal[depth_attachment_format],
                               width,
                               height);
         glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_RENDERBUFFER,
                                   rbo);
      }

      GLenum complete_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (complete_status != GL_FRAMEBUFFER_COMPLETE) {
         assert(false);
      }

      opengl_error_check();

      width_ = width;
      height_ = height;
      id_ = id;
      depth_attachment_ = rbo;
      for (int32 index = 0; index < MAX_FRAMEBUFFER_ATTACHMENTS; index++) {
         color_attachments_[index] = textures[index];
      }

      if (color_attachment_count) {
         GLenum attachment_indices[MAX_FRAMEBUFFER_ATTACHMENTS] = {
            GL_COLOR_ATTACHMENT0 + 0,
            GL_COLOR_ATTACHMENT0 + 1,
            GL_COLOR_ATTACHMENT0 + 2,
            GL_COLOR_ATTACHMENT0 + 3,
         };
         glDrawBuffers(color_attachment_count, attachment_indices);
      }

      return true;
   }

   void framebuffer::destroy() {
      if (!is_valid()) {
         return;
      }

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      width_ = 0;
      height_ = 0;

      glDeleteFramebuffers(1, &id_);
      id_ = 0;

      if (depth_attachment_) {
         glDeleteRenderbuffers(1, &depth_attachment_);
      }
      depth_attachment_ = 0;

      for (int index = 0; index < MAX_FRAMEBUFFER_ATTACHMENTS; index++) {
         if (color_attachments_[index]) {
            glDeleteTextures(1, color_attachments_ + index);
            color_attachments_[index] = 0;
         }
      }

      opengl_error_check();
   }

   void framebuffer::bind() {
      if (!is_valid()) {
         return;
      }

      glBindFramebuffer(GL_FRAMEBUFFER, id_);
      glViewport(0, 0, width_, height_);
   }

   void framebuffer::blit(int32 x, int32 y, int32 width, int32 height) {
      glBindFramebuffer(GL_READ_FRAMEBUFFER, id_);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBlitFramebuffer(0, 0, width_, height_,
                        x, y, x + width, y + height,
                        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                        GL_NEAREST);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
   }
} // !neon
