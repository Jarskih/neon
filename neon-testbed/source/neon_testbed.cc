// neon_testbed.cc

#include "neon_testbed.h"
#include <cassert>

#pragma warning(push)
#pragma warning(disable: 4201)
#pragma warning(disable: 4127)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

namespace neon {
   // note: application factory
   application *application::create(int &width, int &height, string &title) {
      width = 1280; height = 720;
      title = "neon-testbed";
      return new testbed;
   }

   namespace opengl
   {
	   GLuint create_shader(GLenum type, const char* source)
	   {
		   GLuint id = glCreateShader(type);
		   glShaderSource(id, 1, &source, nullptr);
		   glCompileShader(id);
		   return id;
	   }

	   GLuint create_program(GLuint vid, GLuint fid)
	   {
		   GLuint id = glCreateProgram();
		   glAttachShader(id, vid);
		   glAttachShader(id, fid);
		   glLinkProgram(id);

		   GLint status = 0;
		   glGetProgramiv(id, GL_LINK_STATUS, &status);
		   if (status == GL_FALSE)
		   {
			   char vsh_err[1024] = {};
			   char fsh_err[1024] = {};
			   char sh_err[1024] = {};

			   glGetShaderInfoLog(vid, sizeof(vsh_err), nullptr, vsh_err);
			   glGetShaderInfoLog(fid, sizeof(fsh_err), nullptr, fsh_err);
			   glGetShaderInfoLog(id, sizeof(sh_err), nullptr, sh_err);

			   assert(false);

			   return 0;
		   }

		   glDeleteShader(vid);
		   glDeleteShader(fid);

		   return id;
	   }
   } //!opengl


   // note: derived application class
   testbed::testbed() : rotation_(0)
   {
   }
   
   bool testbed::enter() 
   {
	   GLuint vao = 0; //Vertex array object
	   glGenVertexArrays(1, &vao);
	   glBindVertexArray(vao);

	   glEnable(GL_DEPTH_TEST);

	   vertex vertices[] =
	   {
		   //Yellow Triangle
		   /*
			{  0.0,   1.0f,	 0.0f,	0xff00ffff,		0.5f, 1.0f}, // UPPER
			{  1.0f,  0.0f,	 0.0f,	0xff00ffff,		1.0f, 0.0f}, // RIGHT
			{ -1.0f,  0.0f,  0.0f,	0xff00ffff,		0.0f, 0.0f}, // LEFT
			*/

		   // cube 

		   // front 1
		   { 0.0f,  0.0f, 1.0f, 0xff00ffff,		0.5f, 1.0f}, //0
		   { 1.0f,  0.0f, 1.0f, 0xff00ffff,		0.5f, 1.0f}, //1
		   { 1.0f,  1.0f, 1.0f, 0xff00ffff,		0.5f, 1.0f}, //2
		   // front 2
		   { 1.0f,  1.0f, 1.0f, 0xff00ffff,		0.5f, 1.0}, //2
		   { 0.0f,  1.0f, 1.0f, 0xff00ffff,		0.5f, 1.0}, //3
		   { 0.0f,  0.0f, 1.0f, 0xff00ffff,		0.5f, 1.0}, //0
		   // right side 1
		   { 1.0f, 0.0f, 1.0f, 0xff00ffff,		1.0f, 0.0f}, //1
		   { 1.0f, 0.0f, 0.0f, 0xff00ffff,		1.0f, 0.0f}, //5
		   { 1.0f, 1.0f, 0.0f, 0xff00ffff,		1.0f, 0.0f}, //6
		   // right side 2
		   { 1.0f, 0.0f, 1.0f, 0xff00ffff,		1.0f, 0.0f}, //1
		   { 1.0f, 1.0f, 0.0f, 0xff00ffff,		1.0f, 0.0f}, //6
		   { 1.0f, 1.0f, 1.0f, 0xff00ffff,		1.0f, 0.0f}, //2
		  // left side 1
		   {0.0f, 0.0f, 1.0f, 0xff00ffff,		0.0f, 0.0}, //0
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 0.0}, //4
		   {0.0f, 1.0f, 1.0f, 0xff00ffff,		0.0f, 0.0}, //3
		  // left side 2
		   {0.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 0.0}, //7
		   {0.0f, 1.0f, 1.0f, 0xff00ffff,		0.0f, 0.0}, //3
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 0.0}, //4

		  // bottom 1
		   { 0.0f,  0.0f, 1.0f, 0xff00ffff,		0.5f, 1.0f}, //0
		   { 1.0f,  0.0f, 1.0f, 0xff00ffff,		0.5f, 1.0f}, //1
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.5f, 1.0},  //4
		  // bottom 2
		   { 1.0f,  0.0f, 1.0f, 0xff00ffff,		0.5f, 1.0f}, //1
	       { 1.0f, 0.0f, 0.0f, 0xff00ffff,		0.5f, 1.0f}, //5
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.5f, 1.0}, //4

		  // back 1
		   { 1.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 1.0f}, //6
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 1.0f}, //4
		   { 1.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 1.0f}, //5
		  // back 2									  
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 1.0f}, //4
		   { 1.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 1.0f}, //6
		   {0.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 1.0f}, //7

		   //top 1
		  {0.0f, 1.0f, 1.0f, 0xff00ffff,		0.0f, 0.0}, //3
		  {1.0f, 1.0f, 1.0f, 0xff00ffff,		0.0f, 0.0f}, //2
		  { 1.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 0.0f}, //6
		   //top 2
		  { 1.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 0.0f}, //6
		  {0.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 0.0f}, //7
		  {0.0f, 1.0f, 1.0f, 0xff00ffff,		0.0f, 0.0}, //3
	   };
	   
	   if (!vbo_.create(sizeof(vertices), vertices))
	   {
		   return false;
	   }

	   if (!program_.create("assets/vertex_shader.txt", "assets/fragment_shader.txt"))
	   {
		   return false;
	   }

	   format_.add_attribute(0, 3, GL_FLOAT, false);
	   format_.add_attribute(1, 4, GL_UNSIGNED_BYTE, true);
	   format_.add_attribute(2, 2, GL_FLOAT, false);

	   // Create texture
	   if (!texture_.create("")) {
		   return false;
	   }

	   // Create sampler
	   if (!sampler_.create(GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
		   return false;
	   }

	   //Note: uniforms

	   GLfloat aspect = 16.0f / 9.0f;
	   glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.5f, 100.0f);

	   glm::mat4 world = glm::translate(glm::mat4(1.0f), 
										glm::vec3(0.0f, 0.0f, -5.0f));

	   program_.bind();
	   program_.set_uniform_mat4("projection", projection);
	   program_.set_uniform_mat4("world", world);
	

	   GLenum error = glGetError();
	   if (error != GL_NO_ERROR)
	   {
		   // ...
	   };

      return true;
   }

   void testbed::exit() {
   }

   bool testbed::tick(const time &dt) {
      if (keyboard_.is_pressed(KEYCODE_ESCAPE)) {
         return false;
      }

	  rotation_ += dt.as_seconds();

	  glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

	  world = glm::rotate(world, rotation_, glm::vec3(1.0f, 1.0f, 0.0f));

	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  program_.bind();
	  program_.set_uniform_mat4("world", world);

	  vbo_.bind();
	  format_.bind();
	  texture_.bind();
	  sampler_.bind();
	  glDrawArrays(GL_TRIANGLES, 0, 36);

      return true;
   }
} // !neon
