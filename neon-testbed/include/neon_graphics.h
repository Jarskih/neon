//neon_graphics.h

#ifndef NEON_GRAPHICS_H_INCLUDED
#define NEON_GRAPHICS_H_INCLUDED

#include <neon_core.h>
#include <neon_opengl.h>

#pragma warning(push)
#pragma warning(disable: 4201)
#pragma warning(disable: 4127)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

namespace neon
{	
	struct vertex_buffer
	{
		vertex_buffer();

		bool create(const int size, const void* data);
		void destroy();
		bool update(int size, const void* data);

		void bind() const;
		bool is_valid() const;
		// primitive: Specifies what kind of primitives to render. Symbolic constants for example: GL_TRIANGLES
		void render(GLenum primitive, int start, int count);

		GLuint id_;
	};

	struct index_buffer 
	{
		index_buffer();

		bool create(const int size, const GLenum type, const void* data);
		void destroy();

		bool is_valid() const;
		void bind() const;
		void render(GLenum primitive, int start, int count);

		GLuint id_;
		GLenum type_; // Note: type -> GL_UNSIGNED_SHORT or GL_UNSIGNED_INT
	};

	struct shader_program
	{
		shader_program();

		bool create(const string& vertex_shader_filename, const string& fragment_shader_filename);
		void destroy();

		GLint get_attrib_location(const string &name) const;
		GLint get_uniform_location(const string &name) const;

		bool set_uniform_mat4(const string& name, const glm::mat4 &value);
		bool set_uniform_vec4(const string& name, const glm::vec4 &value);

		bool set_uniform_vec3(const string& name, const glm::vec3& value);

		bool is_valid() const;
		void bind() const;

		GLuint id_;
	};

	constexpr uint32 ATTRIBUTE_COUNT = 4;

	struct vertex_format //Describes the things the vertex buffer needs
	{
		struct attribute
		{
			attribute();

			int32 index_;
			uint32 size_;
			GLenum type_;
			bool normalized_;
			uint64 offset_;
		};

		vertex_format();

		void add_attribute(const int32 index, const uint32 size, const GLenum type, const bool normalized);

		bool is_valid() const;
		void bind() const;

		uint32 stride_;
		uint32 attribute_count_;
		attribute attributes_[ATTRIBUTE_COUNT];
	};

	struct texture {
		texture();

		bool create(const std::string& filename, bool flip = true);
		bool createColorTexture(int width, int height);
		bool createDepthTexture(int width, int height);
		bool create_cubemap(int width, int height, const void **data);
		void destroy();
		
		bool is_valid() const;
		void bind();

		GLuint id_;
		GLenum type_;
	};

	struct sampler_state {
		sampler_state();

		bool create(const GLenum filter, const GLenum address_mode_u, const GLenum address_mode_v);
		void destroy();

		bool is_valid();
		void bind();

		GLuint id_;
	};

	struct bitmap_font {
		struct vertex {
			float x_, y_;
			float u_, v_;
		};

		bitmap_font();

		bool create(const string vertex, const string fragment, const string font);
		void destroy();

		void render_text(const float p_x, const float p_y, const string& text);
		void flush();

		shader_program program_;
		vertex_format format_;
		vertex_buffer buffer_;
		texture texture_;
		sampler_state sampler_;
		dynamic_array<vertex> vertices_;
		glm::mat4 projection_;
	};

	

	struct plane {
	   enum plane_type_id {
	      PLANE_NEAR,
	      PLANE_FAR,
	      PLANE_LEFT,
	      PLANE_RIGHT,
	      PLANE_TOP,
	      PLANE_BOTTOM,
	      PLANE_COUNT,
	   };

		plane();

		glm::vec3 normal_;
		float d_;
	};

	struct frustum {
		frustum();

		void construct_from_view_matrix(const glm::mat4 &view);

		plane planes_[plane::PLANE_COUNT];
	};

	struct directional_light {
		directional_light();

		bool create(glm::vec4 color, glm::vec3 direction);
		void update_projection(const frustum &frustum);
		glm::vec3 direction_;
		glm::vec4 color_;
		glm::mat4 projection_{1};

	};

	struct fps_camera {
		fps_camera();

		void update();

		void set_perspective(float fov, float aspect, float znear, float zfar);
		void rotate_x(float amount);
		void rotate_y(float amount);
		void rotate_z(float amount);
		void forward(float amount);
		void sidestep(float amount);

		float yaw_;
		float pitch_;
		float roll_;

		glm::vec3 x_axis;
		glm::vec3 y_axis;
		glm::vec3 z_axis;
		glm::vec3 position_;

		glm::mat4 projection_;
		glm::mat4 view_;
	};

	struct fps_camera_controller {
		fps_camera_controller(fps_camera& camera, keyboard& kb, mouse& m);
		
		void update(const time &deltatime);

		fps_camera& camera_;
		keyboard& keyboard_;
		mouse& mouse_;
		glm::vec2 mouse_position_;
	};

	struct skybox {
		skybox();

		bool create();
		void destroy();

		void render(const fps_camera& camera);
	
		shader_program program_;
		vertex_buffer buffer_;
		vertex_format format_;
		sampler_state sampler_;
		texture cubemap_;
	};

} //!neon

#endif // !NEON_GRAPHICS_H_INCLUDED
