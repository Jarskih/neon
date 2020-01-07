// neon_testbed.cc

#include "neon_testbed.h"
#include <cassert>

#pragma warning(push)
#pragma warning(disable: 4201)
#pragma warning(disable: 4127)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

//#include <imgui/imgui.h>
//#include <imgui/imgui_impl_glfw.h>
//#include <imgui/imgui_impl_opengl3.h>

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
   testbed::testbed() : rotation_(0.0f), controller_(camera_, keyboard_, mouse_)
   {
   }
   
   bool testbed::enter() 
   {
	   GLuint vao = 0; //Vertex array object
	   glGenVertexArrays(1, &vao);
	   glBindVertexArray(vao);

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
		   { 0.0f,  0.0f, 1.0f, 0xff00ffff,		0.0f, 0.0f}, //0
		   { 1.0f,  0.0f, 1.0f, 0xff00ffff,		1.0f, 0.0f}, //1
		   { 1.0f,  1.0f, 1.0f, 0xff00ffff,		1.0f, 1.0f}, //2
		   // front 2
		   { 1.0f,  1.0f, 1.0f, 0xff00ffff,		1.0f, 1.0}, //2
		   { 0.0f,  1.0f, 1.0f, 0xff00ffff,		0.0f, 1.0}, //3
		   { 0.0f,  0.0f, 1.0f, 0xff00ffff,		0.0f, 0.0}, //0
		   // right side 1
		   { 1.0f, 0.0f, 1.0f, 0xff00ffff,		0.0f, 0.0f}, //1
		   { 1.0f, 0.0f, 0.0f, 0xff00ffff,		1.0f, 0.0f}, //5
		   { 1.0f, 1.0f, 0.0f, 0xff00ffff,		1.0f, 1.0f}, //6
		   // right side 2
		   { 1.0f, 0.0f, 1.0f, 0xff00ffff,		0.0f, 0.0f}, //1
		   { 1.0f, 1.0f, 0.0f, 0xff00ffff,		1.0f, 1.0f}, //6
		   { 1.0f, 1.0f, 1.0f, 0xff00ffff,		0.0f, 1.0f}, //2
		  // left side 1
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 0.0}, //4
		   {0.0f, 0.0f, 1.0f, 0xff00ffff,		1.0f, 0.0}, //0
		   {0.0f, 1.0f, 1.0f, 0xff00ffff,		1.0f, 1.0}, //3
		  // left side 2
		   {0.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 1.0}, //7
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 0.0}, //4
		   {0.0f, 1.0f, 1.0f, 0xff00ffff,		1.0f, 1.0}, //3

		  // bottom 1
			{ 1.0f,  0.0f, 1.0f, 0xff00ffff,	1.0f, 0.0f}, //1
		   { 0.0f,  0.0f, 1.0f, 0xff00ffff,		0.0f, 0.0f}, //0
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 0.0f},  //4
		  // bottom 2
		   { 1.0f,  0.0f, 1.0f, 0xff00ffff,		1.0f, 0.0f}, //1
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 0.0}, //4
	       { 1.0f, 0.0f, 0.0f, 0xff00ffff,		1.0f, 0.0f}, //5

		  // back 1
		   { 1.0f, 1.0f, 0.0f, 0xff00ffff,		1.0f, 1.0f}, //6
		   { 1.0f, 0.0f, 0.0f, 0xff00ffff,		1.0f, 0.0f}, //5
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 0.0f}, //4
		  // back 2									  
		   {0.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 1.0f}, //7
		   { 1.0f, 1.0f, 0.0f, 0xff00ffff,		1.0f, 1.0f}, //6
		   {0.0f, 0.0f, 0.0f, 0xff00ffff,		0.0f, 0.0f}, //4

		   //top 1
		  {0.0f, 1.0f, 1.0f, 0xff00ffff,		0.0f, 1.0}, //3
		  {1.0f, 1.0f, 1.0f, 0xff00ffff,		1.0f, 1.0f}, //2
		  { 1.0f, 1.0f, 0.0f, 0xff00ffff,		1.0f, 1.0f}, //6
		   //top 2
		  { 1.0f, 1.0f, 0.0f, 0xff00ffff,		1.0f, 1.0f}, //6
		  {0.0f, 1.0f, 0.0f, 0xff00ffff,		0.0f, 1.0f}, //7
		  {0.0f, 1.0f, 1.0f, 0xff00ffff,		0.0f, 1.0}, //3
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
	   if (!texture_.create("assets/test.png")) {
		   return false;
	   }

	   // Create sampler
	   if (!sampler_.create(GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
		   return false;
	   }

	   // Create light
	   {
		   float near_plane = 1.0f, far_plane = 7.5f;
		   if (!light_.create(glm::vec4(1), glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane), glm::vec3(0,1,0))) {
			   return false;
		   }
	   }

	   // Create shadow frame buffer
	   //if (!shadow_frame_buffer_.create(128, 128)) {
	  //	   return false;
	  // }

	   //Note: uniforms
	   {
		   GLfloat aspect = 16.0f / 9.0f;
		   glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.5f, 100.0f);

		   glm::mat4 world = glm::translate(glm::mat4(1.0f),
			   glm::vec3(0.0f, 0.0f, -5.0f));

		   program_.bind();
		   program_.set_uniform_mat4("projection", projection);
		   program_.set_uniform_mat4("world", world);
	   }

	   GLenum error = glGetError();
	   if (error != GL_NO_ERROR)
	   {
		   // ...
	   };

	   // Create text font
	   if (!font_.create()) {
		   return false;
	   };

	   if (!skybox_.create()) {
		   return false;
	   };

	   if (!terrain_.create("assets/heightmap/heightmap.png", "assets/heightmap/texture.png")) {
		   return false;
	   }

	   if (!terrain2_.create("assets/heightmap/heightmap.png", "assets/heightmap/texture.png")) {
		   return false;
	   }
	   terrain2_.position_ = glm::vec3(0.0f, 50.0f, 0.0f);

	   // Planets
	   sun_.position_ = glm::vec3(0.0f, 0.0f, 0.0f);

	   if (!sun_.create("assets/sphere/2k_sun.jpg", 30, 36, 36)) {
		   return false;
	   }

	   mercury_.position_ = glm::vec3(58.0f, 0.0f, 0.0f);
	   if (!mercury_.create("assets/sphere/2k_mercury.jpg", 2.440f, 36, 36)) {
		   return false;
	   }

	   venus_.position_ = glm::vec3(108.0f, 0.0f, 0.0f);
	   if (!venus_.create("assets/sphere/2k_venus_atmosphere.jpg", 6.052f, 36, 36)) {
		   return false;
	   }

	   earth_.position_ = glm::vec3(150.0f, 0.0f, 0.0f);
	   if (!earth_.create("assets/sphere/2k_earth_daymap.jpg", 6.378f, 36, 36)) {
	 	   return false;
	   }

	   moon_.position_ = glm::vec3(15.0f, 0.0f, 0.0f); // Relative to parent (earth)
	   moon_.pivot_ = earth_.position_;
	   moon_.isMoon_ = true;
	   if (!moon_.create("assets/sphere/2k_moon.jpg", 1.0f, 36, 36)) {
		   return false;
	   }

	   mars_.position_ = glm::vec3(227.0f, 0.0f, 0.0f);
	   if (!mars_.create("assets/sphere/2k_mars.jpg", 3.397f, 36, 36)) {
		   return false;
	   }

	   jupiter_.position_ = glm::vec3(778.0f, 0.0f, 0.0f);
	   if (!jupiter_.create("assets/sphere/2k_jupiter.jpg", 71.492f, 36, 36)) {
		   return false;
	   }

	   saturn_.position_ = glm::vec3(1434.0f, 0.0f, 0.0f);
	   if (!saturn_.create("assets/sphere/2k_saturn.jpg", 60.268f, 36, 36)) {
		   return false;
	   }

	   uranus_.position_ = glm::vec3(2871.0f, 0.0f, 0.0f);
	   if (!uranus_.create("assets/sphere/2k_uranus.jpg", 25.559f, 36, 36)) {
		   return false;
	   }

	   neptune_.position_ = glm::vec3(4496.0f, 0.0f, 0.0f);
	   if (!neptune_.create("assets/sphere/2k_neptune.jpg", 24.766f, 36, 36)) {
		   return false;
	   }

	   camera_.set_perspective(45.0f, 16.0f / 9.0f, 0.5f, 1000.0f);

      return true;
   }

   void testbed::exit() {
   }

   bool testbed::tick(const time &dt) {
      if (keyboard_.is_pressed(KEYCODE_ESCAPE)) {
         return false;
      }

	  // Update camera
	  controller_.update(dt);

	  string text = "dt: " + std::to_string(dt.as_milliseconds()) + " (FPS:" + std::to_string(1.0f/dt.as_seconds()) + ")";
	  font_.render_text(2.0f, 2.0f, text);

	  // clear
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  skybox_.render(camera_);

	  // render shadow map to frame buffer

	  terrain_.render(camera_, light_);

	  terrain2_.render(camera_, light_);

 	  //shadow_frame_buffer_.bind();
	  //shadow_frame_buffer_.render(camera_);
	  //shadow_frame_buffer_.unbind();

	  // render normally


	  /*
	  terrain_.render(camera_, light_);
	  earth_.render(camera_, dt);
	  moon_.pivot_ = earth_.position_;
	  moon_.render(camera_, dt);
	  mars_.render(camera_, dt);
	  jupiter_.render(camera_, dt);
	  mercury_.render(camera_, dt);
	  neptune_.render(camera_, dt);
	  saturn_.render(camera_, dt);
      uranus_.render(camera_, dt);
      venus_.render(camera_, dt);
	  sun_.render(camera_, dt);
	  */

	  // Draw text
	  font_.flush();

      return true;
   }
} // !neon
