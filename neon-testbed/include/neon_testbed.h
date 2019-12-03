// neon_testbed.h

#ifndef NEON_TESTBED_H_INCLUDED
#define NEON_TESTBED_H_INCLUDED

#include <neon_core.h>
#include <neon_opengl.h>

#include "neon_graphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Render triangle in 3d: add Z component, add attribute, add vertice for Z

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
	  float rotation_;

	  shader_program program_;
	  vertex_buffer vbo_;
	  vertex_format format_;
	  texture texture_;
	  sampler_state sampler_;
   };
} // !neon

#endif // !NEON_TESTBED_H_INCLUDED
