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
		   if (!light_.create(glm::vec4(1), glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane), glm::vec3(0,1,0))) {
			   return false;
		   }
	   }

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

	   // Planets
	   sun_.position_ = glm::vec3(0.0f, 0.0f, 0.0f);

	   if (!sun_.create("assets/sphere/2k_sun.jpg", 30, 36, 36, 0.01f)) {
		   return false;
	   }

	   mercury_.position_ = glm::vec3(58.0f, 0.0f, 0.0f);
	   if (!mercury_.create("assets/sphere/2k_mercury.jpg", 2.440f, 36, 36, 180)) {
		   return false;
	   }

	   venus_.position_ = glm::vec3(108.0f, 0.0f, 0.0f);
	   if (!venus_.create("assets/sphere/2k_venus_atmosphere.jpg", 6.052f, 36, 36, 240)) {
		   return false;
	   }

	   earth_.position_ = glm::vec3(150.0f, 0.0f, 0.0f);
	   if (!earth_.create("assets/sphere/2k_earth_daymap.jpg", 6.378f, 36, 36, 365)) {
	 	   return false;
	   }

	   moon_.position_ = glm::vec3(15.0f, 0.0f, 0.0f); // Relative to parent (earth)
	   moon_.pivot_ = earth_.position_;
	   moon_.isMoon_ = true;
	   if (!moon_.create("assets/sphere/2k_moon.jpg", 1.0f, 36, 36, 365)) {
		   return false;
	   }

	   mars_.position_ = glm::vec3(227.0f, 0.0f, 0.0f);
	   if (!mars_.create("assets/sphere/2k_mars.jpg", 3.397f, 36, 36, 400)) {
		   return false;
	   }

	   jupiter_.position_ = glm::vec3(300.0f, 0.0f, 0.0f);
	   if (!jupiter_.create("assets/sphere/2k_jupiter.jpg", 71.492f, 36, 36, 500)) {
		   return false;
	   }

	   saturn_.position_ = glm::vec3(434.0f, 0.0f, 0.0f);
	   if (!saturn_.create("assets/sphere/2k_saturn.jpg", 60.268f, 36, 36, 600)) {
		   return false;
	   }

	   uranus_.position_ = glm::vec3(571.0f, 0.0f, 0.0f);
	   if (!uranus_.create("assets/sphere/2k_uranus.jpg", 25.559f, 36, 36, 700)) {
		   return false;
	   }

	   neptune_.position_ = glm::vec3(600, 0.0f, 0.0f);
	   if (!neptune_.create("assets/sphere/2k_neptune.jpg", 24.766f, 36, 36, 800)) {
		   return false;
	   }

	   if (!model_.create_from_file("assets/model/Chest.FBX", "assets/model/vertex_shader.txt", "assets/model/fragment_shader.txt", "assets/model/diffuse.png")) {
		   return false;
	   }

	   model_matrix_ = glm::translate(glm::mat4(1), glm::vec3(50.0f, 30.0f, 50.0f));
	   model_matrix_ = glm::scale(model_matrix_, glm::vec3(0.2f));

	   camera_.set_perspective(45.0f, 16.0f / 9.0f, 0.5f, 10000.0f);
	   camera_.position_ = { 0, 0, 1000 };

	   framebuffer_format formats[] = { FRAMEBUFFER_FORMAT_RGBA8 };

	   if (!framebuffer_.create(240, 132, _countof(formats), formats, FRAMEBUFFER_FORMAT_D32)) {
		   return false;
	   }

      return true;
   }

   void testbed::exit() {
   }

   bool testbed::tick(const time &dt) {
      if (keyboard_.is_pressed(KEYCODE_ESCAPE)) {
         return false;
      }

	  if (keyboard_.is_down(KEYCODE_Z)) {
		  light_.position_ = glm::vec3(light_.position_.x + dt.as_seconds() * 10.0f, light_.position_.y, light_.position_.z);
		  light_.view_ = glm::lookAt(light_.position_, glm::vec3(0), glm::vec3(0, 1, 0));
	  }

	  if (keyboard_.is_down(KEYCODE_X)) {
		  light_.position_ = glm::vec3(light_.position_.x - dt.as_seconds() * 10.0f, light_.position_.y, light_.position_.z);
		  light_.view_ = glm::lookAt(light_.position_, glm::vec3(0), glm::vec3(0, 1, 0));
	  }

	  if (keyboard_.is_down(KEYCODE_C)) {
		  light_.position_ = glm::vec3(light_.position_.x, light_.position_.y + dt.as_seconds() * 10.0f, light_.position_.z);
		  light_.view_ = glm::lookAt(light_.position_, glm::vec3(0), glm::vec3(0, 1, 0));
	  }

	  if (keyboard_.is_down(KEYCODE_V)) {
		  light_.position_ = glm::vec3(light_.position_.x, light_.position_.y - dt.as_seconds() * 10.0f, light_.position_.z);
		  light_.view_ = glm::lookAt(light_.position_, glm::vec3(0), glm::vec3(0, 1, 0));
	  }


	  // Update camera
	  controller_.update(dt);

	  // first pass
	  framebuffer_.bind();
	  // clear
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  terrain_.render_shadow_map(light_);
	  model_.render_shadow_map(light_);

	  framebuffer::unbind(1280, 720);

	  // second pass
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  string text = "dt: " + std::to_string(dt.as_seconds());
	  font_.render_text(2.0f, 2.0f, text);

	  framebuffer_.bind_as_depth(1);

	  skybox_.render(camera_);
	  terrain_.render(camera_, light_);
	  model_.render(camera_, model_matrix_);

	 // framebuffer_.blit(0, 0, 1280, 720);


	  /*
		correct rotation
	    float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	  */

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

	  // Draw text
	  font_.flush();

      return true;
   }
} // !neon
