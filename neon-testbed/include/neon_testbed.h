// neon_testbed.h

#ifndef NEON_TESTBED_H_INCLUDED
#define NEON_TESTBED_H_INCLUDED

#include <neon_core.h>
#include <neon_opengl.h>

#include "neon_graphics.h"
#include "neon_framebuffer.h"
#include "neon_sphere.h"

namespace neon 
{
	struct vertex 
	{
		float x_;
		float y_;
		float z_;
		uint32 color_;
		float u_, v_;
	};

   struct testbed : application 
   {
      testbed();
      virtual bool enter() final;
      virtual void exit() final;
      virtual bool tick(const time &dt) final;

	  shader_program program_;
	  vertex_buffer vbo_;
	  index_buffer index_buffer_;
	  vertex_format format_;
	  texture texture_;
	  sampler_state sampler_;

	  float rotation_;
	  bitmap_font font_;
	  bitmap_font signed_font_;

   	  frustum frustum_;
	  fps_camera camera_;
	  directional_light light_;
	  fps_camera_controller controller_;

	  framebuffer framebuffer_;
   	
	  sphere earth_;
	  sphere jupiter_;
	  sphere saturn_;
	  sphere sun_;
   };
} // !neon

#endif // !NEON_TESTBED_H_INCLUDED
