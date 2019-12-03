//neon_graphics.cc

#include "neon_graphics.h"
#include <cassert>

namespace neon
{

	vertex_buffer::vertex_buffer()
		: id_(0)
	{

	};

	bool vertex_buffer::create(const int size, const void* data)
	{
		if (is_valid())
		{
			return false;
		}

		glGenBuffers(1, &id_);
		glBindBuffer(GL_ARRAY_BUFFER, id_);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		GLenum err = glGetError();

		return err == GL_NO_ERROR;
	}

	void vertex_buffer::destroy()
	{
		if (!is_valid())
		{
			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &id_); //Deletes space and handle.
		id_ = 0;
	}

	GLint shader_program::get_attrib_location(const string &name) const {
		bind();
		return glGetAttribLocation(id_, name.c_str()); // Returns -1 if fails
	}

	GLint shader_program::get_uniform_location(const string &name) const {
		bind();
		return glGetUniformLocation(id_, name.c_str()); // Returns -1 if fails
	}

	bool shader_program::set_uniform_mat4(const string &name, const glm::mat4 &value) {
		GLuint location = get_uniform_location(name);

		if (location == -1) {
			return false;
		}

		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));

		return true;
	}

	bool shader_program::set_uniform_vec4(const string &name, const glm::vec4 &value) {
		GLuint location = get_uniform_location(name);

		if (location == -1) {
			return false;
		}

		glUniform4fv(location, 1, glm::value_ptr(value));

		return true;
	}

	bool vertex_buffer::is_valid() const
	{
		return id_ != 0;
	}

	void vertex_buffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, id_);
	}

	namespace { //Anon namespace
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

			GLint status = GL_TRUE;
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

				glDeleteProgram(id);
				id = 0;
			}

			glDeleteShader(vid);
			glDeleteShader(fid);

			return id;
		}

	} //anon

	shader_program::shader_program()
		:id_(0)
	{
	}

	bool shader_program::create(const string& vertex_shader_filename, const string& fragment_shader_filename)
	{
		if (is_valid())
		{
			return false;
		}

		dynamic_array<uint8> vertex_shader_file_content;
		if (!file_system::read_file_content(vertex_shader_filename, vertex_shader_file_content))
		{
			return false;
		}
		vertex_shader_file_content.push_back(0); //All strings are 0 terminated. It stops reading at \0 
		const char* vertex_shader_source = (const char*)vertex_shader_file_content.data();

		dynamic_array<uint8> fragment_shader_file_content;
		if (!file_system::read_file_content(fragment_shader_filename, fragment_shader_file_content))
		{
			return false;
		}
		fragment_shader_file_content.push_back(0);
		const char* fragment_shader_source = (const char*)fragment_shader_file_content.data();


		GLuint vid = create_shader(GL_VERTEX_SHADER, vertex_shader_source);
		GLuint fid = create_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
		id_ = create_program(vid, fid);

		GLenum error = glGetError();
		return error == GL_NO_ERROR;
	}

	void shader_program::destroy()
	{
		if (!is_valid())
		{
			return;
		}

		glDeleteProgram(id_);
		id_ = 0;
	}

	bool shader_program::is_valid() const
	{
		return id_ != 0;
	}

	void shader_program::bind() const
	{
		glUseProgram(id_);
	}

	vertex_format::attribute::attribute()
		: index_(-1), size_(0), type_(0), normalized_(false), offset_(0)
	{
	}

	vertex_format::vertex_format()
		: stride_(0), attribute_count_(0), attributes_{}
	{
	}

	namespace
	{
		uint32 size_of_gl_type(GLenum type)
		{
			switch (type)
			{
			case GL_FLOAT:
				return sizeof(GLfloat);
			case GL_UNSIGNED_BYTE:
				return sizeof(uint8);
			}
			assert(false);
			return 0;
		}
	} //!Anon

	void vertex_format::add_attribute(const int32 index, const uint32 size, const GLenum type, const bool normalized)
	{
		assert(attribute_count_ < ATTRIBUTE_COUNT); //Trying to add more attributes than allowed

		uint32 at = attribute_count_++;
		attributes_[at].index_ = index;
		attributes_[at].size_ = size;
		attributes_[at].type_ = type;
		attributes_[at].normalized_ = normalized;
		attributes_[at].offset_ = stride_;

		stride_ += size * size_of_gl_type(type); //How many steps I have to go until I find the next index.

	}

	bool vertex_format::is_valid() const
	{
		return attribute_count_ > 0;
	}

	void vertex_format::bind() const
	{
		for (uint32 index = 0; index < attribute_count_; index++)
		{
			glDisableVertexAttribArray(index);	//We don't know the previous vertex format. So we have to disable the previous formats
		}

		for (uint32 index = 0; index < attribute_count_; index++)
		{
			const attribute& attrib = attributes_[index];
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(attrib.index_, attrib.size_, attrib.type_, attrib.normalized_, stride_, (const void*)attrib.offset_);
		}
	}

	texture::texture() : id_(0)
	{
	}

	bool texture::create(const std::string& filename)
	{
		if (is_valid()) {
			return false;
		}

		glGenTextures(1, &id_);

		glBindTexture(GL_TEXTURE_2D, id_);

		uint32 bitmap[] = {
			0xff0000ff, 0xff00ff00,
			0xffff000f, 0xff00ffff,
		};


		// TODO load texture from file 

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
		
		GLenum error = glGetError();
		return error == GL_NO_ERROR;
	}

	void texture::destroy()
	{
		if (!is_valid()) {
			return;
		}

		glDeleteTextures(1, &id_);
		id_ = 0;
	}

	bool texture::is_valid() const
	{
		return false;
	}

	void texture::bind()
	{
		glBindTexture(GL_TEXTURE_2D, id_); // bind 0 to clear bind
	}

	sampler_state::sampler_state() : id_(0)
	{
	}

	bool sampler_state::create(const GLenum filter, const GLenum address_mode_u, const GLenum address_mode_v)
	{
		if (is_valid()) {
			return false;
		}
	
		glGenSamplers(1, &id_);
		glBindSampler(0, id_);
		glSamplerParameteri(id_, GL_TEXTURE_MIN_FILTER, filter);
		glSamplerParameteri(id_, GL_TEXTURE_MAG_FILTER, filter);
		glSamplerParameteri(id_, GL_TEXTURE_WRAP_S, address_mode_u);
		glSamplerParameteri(id_, GL_TEXTURE_WRAP_T, address_mode_v);

		return glGetError() == GL_NO_ERROR;
	}

	void sampler_state::destroy()
	{
		if (!is_valid()) {
			return;
		}

		glDeleteSamplers(1, &id_);
		id_ = 0;
	}

	bool sampler_state::is_valid()
	{
		return false;
	}

	void sampler_state::bind()
	{
		glBindSampler(0, id_);
	}

} //!neon
