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

		GLuint id_;
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

		bool create(const std::string& filename);
		void destroy();
		
		bool is_valid() const;
		void bind();

		GLuint id_;
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

		bool create();
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

} //!neon

#endif // !NEON_GRAPHICS_H_INCLUDED
