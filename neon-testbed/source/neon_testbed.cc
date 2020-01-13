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

	   // Create light
	   {
		   glm::vec4 white = glm::vec4(1);
		   if (!light_.create(white, glm::vec3(0,1,0))) {
			   return false;
		   }
	   }

	   // Create text font
	   if (!font_.create("assets/fonts/bitmap_font_vertex.shader", "assets/fonts/bitmap_font_fragment.shader", "assets/fonts/font_8x8.png")) {
		   return false;
	   };

	   if (!skybox_.create()) {
		   return false;
	   };

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

	   moon_.position_ = glm::vec3(15.0f, 0.0f, 0.0f); // Relative to parent
	   moon_.set_parent(earth_.position_);
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

	   camera_.set_perspective(45.0f, 16.0f / 9.0f, 0.5f, 10000.0f);
	   camera_.position_ = { 0, 0, 1000 };

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

	
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  string text = "dt: " + std::to_string(dt.as_milliseconds()) + 
		  " (FPS: " + std::to_string(1.0f / dt.as_seconds()) + ")";
	  font_.render_text(2.0f, 2.0f, text);;

	  skybox_.render(camera_);
	  earth_.render(camera_, dt);
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
