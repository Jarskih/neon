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
		   if (!light_.create(white, glm::vec3(0,-1,0))) {
			   return false;
		   }
	   }

	   // Create framebuffer
	   framebuffer_format formats[] = { FRAMEBUFFER_FORMAT_RGBA8 };

	   if (!framebuffer_.create(240, 132, _countof(formats), formats, FRAMEBUFFER_FORMAT_D32)) {
		   return false;
	   }

	   // Create text font
	   if (!font_.create("assets/fonts/bitmap_font_vertex_shader.shader", "assets/fonts/bitmap_font_fragment_shader.shader", "assets/fonts/font_8x8.png")) {
		   return false;
	   };

	   if (!skybox_.create()) {
		   return false;
	   };

	   if (!terrain_.create("assets/heightmap/heightmap.png", "assets/heightmap/texture.png")) {
		   return false;
	   }

	   // Planets
	   sun_.position_ = glm::vec3(50.0f, 50.0f, 50.0f);

	   if (!sun_.create("assets/sphere/2k_sun.jpg", 30, 36, 36, 0.01f)) {
		   return false;
	   }

	   earth_.position_ = glm::vec3(150.0f, 0.0f, 0.0f);
	   if (!earth_.create("assets/sphere/2k_earth_daymap.jpg", 6.378f, 36, 36, 365)) {
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

	   if (!model_.create_from_file("assets/model/Chest.FBX", "assets/model/vertex_shader.txt", "assets/model/fragment_shader.txt", "assets/model/diffuse.png")) {
		   return false;
	   }

	   model_matrix_ = glm::translate(glm::mat4(1), glm::vec3(50.0f, 30.0f, 50.0f));
	   model_matrix_ = glm::scale(model_matrix_, glm::vec3(0.2f));

	   camera_.set_perspective(45.0f, 16.0f / 9.0f, 0.5f, 10000.0f);
	   camera_.position_ = { 5, 5, 5 };

      return true;
   }

   void testbed::exit() {
   }

   bool testbed::tick(const time &dt) {
      if (keyboard_.is_pressed(KEYCODE_ESCAPE)) {
         return false;
      }

	  if (keyboard_.is_down(KEYCODE_UP)) {
		  light_.direction_ = glm::vec3(light_.direction_.x + dt.as_seconds() * 2.0f, light_.direction_.y, light_.direction_.z);
	  }

	  if (keyboard_.is_down(KEYCODE_DOWN)) {
		  light_.direction_ = glm::vec3(light_.direction_.x - dt.as_seconds() * 2.0f, light_.direction_.y, light_.direction_.z);
	  }

	  if (keyboard_.is_down(KEYCODE_LEFT)) {
		  light_.direction_ = glm::vec3(light_.direction_.x, light_.direction_.y + dt.as_seconds() * 2.0f, light_.direction_.z);
	  }

	  if (keyboard_.is_down(KEYCODE_RIGHT)) {
		  light_.direction_ = glm::vec3(light_.direction_.x, light_.direction_.y - dt.as_seconds() * 2.0f, light_.direction_.z);
	  }

	  if (keyboard_.is_down(KEYCODE_Q)) {
		  light_.color_ = glm::vec4(1, 0.5f, 0.5f, 1);
	  }

	  if (keyboard_.is_down(KEYCODE_E)) {
		  light_.color_ = glm::vec4(1);
	  }


	  // Update camera
	  controller_.update(dt);

	  // first pass
	  framebuffer_.bind();
	  // clear
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  sun_.render_shadow_map(light_, camera_);
	  earth_.render_shadow_map(light_, camera_);
	  jupiter_.render_shadow_map(light_, camera_);
	  saturn_.render_shadow_map(light_, camera_);

	  framebuffer::unbind(1280, 720);

	  // second pass
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  string text = "dt: " + std::to_string(dt.as_milliseconds()) +
		  " (FPS: " + std::to_string(1.0f / dt.as_seconds()) + ")";

	  framebuffer_.bind_as_depth(1);

	  skybox_.render(camera_);
	  sun_.render(camera_, dt, light_);
	  earth_.render(camera_, dt, light_);
	  jupiter_.render(camera_, dt, light_);
	  saturn_.render(camera_, dt, light_);

	  // framebuffer_.blit(0, 0, 1280, 720);

	  // Draw text
	  font_.flush();

      return true;
   }
} // !neon
