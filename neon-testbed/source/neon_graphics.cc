//neon_graphics.cc

#include "neon_graphics.h"
#include <cassert>

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace neon
{
	// Vertex buffer
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

	bool vertex_buffer::update(int size, const void* data)
	{
		if (!is_valid())
		{
			return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, id_);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		return true;
	}

	// Index buffer
	index_buffer::index_buffer() : id_(0), type_(GL_UNSIGNED_SHORT)
	{
	}

	bool index_buffer::create(const int size, const GLenum type, const void* data)
	{
		if (is_valid()) {
			return false;
		}

		type_ = type;

		glGenBuffers(1, &id_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		GLenum error = glGetError();
		return error == GL_NO_ERROR;
	}

	void index_buffer::destroy()
	{
		if (!is_valid()) {
			return;
		}

		glDeleteBuffers(1, &id_);
		id_ = 0;
	}

	bool index_buffer::is_valid() const
	{
		return id_ != 0;
	}

	void index_buffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
	}

	void index_buffer::render(GLenum primitive, int start, int count)
	{
		glDrawElements(primitive, count, type_, (const void*) nullptr);
	}

	// Shader program
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

	bool shader_program::set_uniform_vec3(const string& name, const glm::vec3& value) {
		GLuint location = get_uniform_location(name);

		if (location == -1) {
			return false;
		}

		glUniform3fv(location, 1, glm::value_ptr(value));

		return true;
	}

	bool vertex_buffer::is_valid() const
	{
		return id_ != 0;
	}

	void vertex_buffer::render(GLenum primitive, int start, int count)
	{
		glDrawArrays(primitive, start, count);
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

	texture::texture() : id_(0), type_(0)
	{
	}

	bool texture::create(const std::string& filename, bool flip)
	{
		if (is_valid()) {
			return false;
		}

		// Flip image
		stbi_set_flip_vertically_on_load(flip);

		// read file
		dynamic_array<uint8> file_content;
		if (!file_system::read_file_content(filename, file_content)) {
			return false;
		}

		// save image from memory to bitmap
		int width = 0, height = 0, components = 0;
		auto bitmap = stbi_load_from_memory(file_content.data(), (int)file_content.size(), &width, &height, &components, STBI_rgb_alpha);

		if (!bitmap) {
			return false;
		}

		glGenTextures(1, &id_);

		type_ = GL_TEXTURE_2D;
		glBindTexture(GL_TEXTURE_2D, id_);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);

		stbi_image_free(bitmap);
		
		GLenum error = glGetError();
		return error == GL_NO_ERROR;
	}

	bool texture::createColorTexture(int width, int height)
	{
		if (is_valid()) {
			return false;
		}

		glGenTextures(1, &id_);
		glBindTexture(GL_TEXTURE_2D, id_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, id_, 0);

		GLenum error = glGetError();
		return error == GL_NO_ERROR;
	}

	bool texture::createDepthTexture(int width, int height) {
		if (is_valid()) {
			return false;
		}

		glGenTextures(1, &id_);
		glBindTexture(GL_TEXTURE_2D, id_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT32, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id_, 0);

		GLenum error = glGetError();
		return error == GL_NO_ERROR;
	}

	bool texture::create_cubemap(int width, int height, const void** data)
	{
		if (is_valid()) {
			return false;
		}

		type_ = GL_TEXTURE_CUBE_MAP;
		glGenTextures(1, &id_);

		glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
		
		for (int index = 0; index < 6; index++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[index]);
		}

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
		glBindTexture(type_, id_); // bind 0 to clear bind
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

	bitmap_font::bitmap_font() : projection_(1.0f) {

	}

	bool bitmap_font::create(const string vertex, const string fragment, const string font)
	{
		if (!program_.create(vertex, fragment)) {
			return false;
		}

		format_.add_attribute(0, 2, GL_FLOAT, false);
		format_.add_attribute(1, 2, GL_FLOAT, false);

		if (!buffer_.create(512, nullptr)) {
			return false;
		}

		if (!texture_.create(font, false)) {
			return false;
		}

		if (!sampler_.create(GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
			return false;
		}

		projection_ = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);

		return true;
	}

	void bitmap_font::destroy()
	{
	}

	void bitmap_font::render_text(const float pos_x, const float pos_y, const string& text) {
		
		const int characters_per_row = 16;
		const float size = 8.0f;
		const float uv1 = 1.0f / characters_per_row;
		
		float p_x = pos_x;
		float p_y = pos_y;

		const int offset = 2;
		for (auto &character : text) {
			int index = character - ' ';
			int x = index % characters_per_row;
			int y = index / characters_per_row + offset;

			float u = (float)x / characters_per_row;
			float v = (float)y / characters_per_row;

			vertex vertices[6] = {
				{p_x,        p_y,         u,        v       },
				{p_x + size, p_y,         u + uv1,  v       },
				{p_x + size, p_y + size,  u + uv1,  v + uv1 },

				{p_x + size, p_y + size,  u + uv1,  v + uv1 },
				{p_x,        p_y + size,  u,        v + uv1 },
				{p_x,        p_y,         u,        v       },
			};

			for (auto& vert : vertices) {
				vertices_.push_back(vert);
			}

			p_x += size;
		}
	}

	void bitmap_font::flush()
	{
		// note: submit vertices from CPU to GPU
		int size = (int)(sizeof(vertex) * vertices_.size());
		buffer_.update(size , vertices_.data());

		// note: render the thing!
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		
		program_.bind();
		program_.set_uniform_mat4("projection", projection_);
		buffer_.bind();
		format_.bind();
		texture_.bind();
		sampler_.bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices_.size());

		GLenum error = glGetError();
	
		vertices_.clear();
	}

	frustum::frustum()
	{
	}

    plane::plane(): normal_{}, d_{}
	{
	}

   // source: https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
   void frustum::construct_from_view_matrix(const glm::mat4 &view) {
      // note: extract left plane
      planes_[plane::PLANE_LEFT].normal_  = glm::vec3(view[0][3] + view[0][0],
                                                      view[1][3] + view[1][0],
                                                      view[2][3] + view[2][0]);
      planes_[plane::PLANE_LEFT].d_       = view[3][3] + view[3][0];

      // note: extract right plane
      planes_[plane::PLANE_RIGHT].normal_ = glm::vec3(view[0][3] - view[0][0],
                                                      view[1][3] - view[1][0],
                                                      view[2][3] - view[2][0]);
      planes_[plane::PLANE_RIGHT].d_      = view[3][3] - view[3][0];

      // note: extract top plane
      planes_[plane::PLANE_TOP].normal_   = glm::vec3(view[0][3] - view[0][1],
                                                      view[1][3] - view[1][1],
                                                      view[2][3] - view[2][1]);
      planes_[plane::PLANE_TOP].d_        = view[3][3] - view[3][1];

      // note: extract bottom plane
      planes_[plane::PLANE_BOTTOM].normal_ = glm::vec3(view[0][3] + view[0][1],
                                                       view[1][3] + view[1][1],
                                                       view[2][3] + view[2][1]);
      planes_[plane::PLANE_BOTTOM].d_     = view[3][3] + view[3][1];

      // note: extract far plane
      planes_[plane::PLANE_FAR].normal_   = glm::vec3(view[0][3] - view[0][2],
                                                      view[1][3] - view[1][2],
                                                      view[2][3] - view[2][2]);
      planes_[plane::PLANE_FAR].d_        = view[3][3] - view[3][2];

      // note: extract near plane
      planes_[plane::PLANE_NEAR].normal_  = glm::vec3(view[0][2],
                                                      view[1][2],
                                                      view[2][2]);
      planes_[plane::PLANE_NEAR].d_       = view[3][2];


      for (int32 i = 0; i < plane::PLANE_COUNT; i++) {
         float length = glm::length(planes_[i].normal_);
         planes_[i].normal_ /= length;
         planes_[i].d_ /= length;
      }
   }
	
	directional_light::directional_light() : color_(0), direction_(0)
	{
	}

	bool directional_light::create(glm::vec4 color, glm::vec3 direction) {
		projection_ = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 1000.0f);
		color_ = color;
		direction_ = direction;
		return true;
	}

	void directional_light::update_projection(const frustum &frustum)
	{
		projection_ = glm::ortho(frustum.planes_[plane::PLANE_LEFT].d_, frustum.planes_[plane::PLANE_RIGHT].d_, frustum.planes_[plane::PLANE_BOTTOM].d_, frustum.planes_[plane::PLANE_TOP].d_, frustum.planes_[plane::PLANE_NEAR].d_, frustum.planes_[plane::PLANE_FAR].d_);
	}

	fps_camera::fps_camera() : 
		yaw_(0.0f), 
		pitch_(0.0f), 
		roll_(0.0f), 
		x_axis(0.0f), 
		y_axis(0.0f), 
		z_axis(0.0f), 
		position_(0.0f), 
		projection_(1.0f), 
		view_(1.0f)
	{
	}

	void fps_camera::update()
	{
		glm::vec3 x(1.0f, 0.0f, 0.0f);
		glm::vec3 y(0.0f, 1.0f, 0.0f);
		glm::vec3 z(0.0f, 0.0f, 1.0f);

		// Rotate around Y axis (yaw)
		glm::mat4 ry = glm::rotate(glm::mat4(1.0f), yaw_, y);

		// Avoiding gimbal lock
		x = ry * glm::vec4(x, 0.0f);
		z = ry * glm::vec4(z, 0.0f);

		// Rotate around X axis (pitch)
		glm::mat4 rx = glm::rotate(glm::mat4(1.0f), pitch_, x);
		
		y = rx * glm::vec4(y, 0.0f);
		z = rx * glm::vec4(z, 0.0f);

		// Rotate around Z axis (roll)
		glm::mat4 rz = glm::rotate(glm::mat4(1.0f), roll_, z);

		x = rz * glm::vec4(x, 0.0f);
		y = rz * glm::vec4(y, 0.0f);

		// Create view matrix
		view_ = glm::mat4(1.0f);
		view_[0][0] = x.x; view_[1][0] = x.y; view_[2][0] = x.z;
		view_[0][1] = y.x; view_[1][1] = y.y; view_[2][1] = y.z;
		view_[0][2] = z.x; view_[1][2] = z.y; view_[2][2] = z.z;

		// Translation
		view_[3][0] = -glm::dot(position_, x);
		view_[3][1] = -glm::dot(position_, y);
		view_[3][2] = -glm::dot(position_, z);

		// Note: movement helpers
		x_axis = x;
		y_axis = y;
		z_axis = z;
	}

	void fps_camera::set_perspective(float fov, float aspect, float znear, float zfar)
	{
		// fov in degrees
		projection_ = glm::perspective(glm::radians(fov), aspect, znear, zfar);
	}
	void fps_camera::rotate_x(float amount)
	{
		pitch_ += amount;
	}

	void fps_camera::rotate_y(float amount)
	{
		yaw_ += amount;
	}

	void fps_camera::rotate_z(float amount) 
	{
		roll_ += amount;
	}

	void fps_camera::forward(float amount)
	{
		position_ += z_axis * amount;
	}

	void fps_camera::sidestep(float amount)
	{
		position_ += x_axis * amount;
	}

	fps_camera_controller::fps_camera_controller(fps_camera& camera, keyboard& kb, mouse& m) : camera_(camera), keyboard_(kb), mouse_(m), mouse_position_()
	{
	}

	void fps_camera_controller::update(const time& deltatime)
	{
		constexpr float camera_speed = 50.0f;
		constexpr float camera_turn_speed = 5.0f;
		const float amount = camera_speed * deltatime.as_seconds();

		// Camera Movement
		if (keyboard_.is_down(KEYCODE_W)) {
			camera_.forward(-amount * deltatime.as_seconds() * camera_speed);
		}

		if (keyboard_.is_down(KEYCODE_S)) {
			camera_.forward(amount * deltatime.as_seconds() * camera_speed);
		}

		if (keyboard_.is_down(KEYCODE_A)) {
			camera_.sidestep(-amount * deltatime.as_seconds() * camera_speed);
		}

		if (keyboard_.is_down(KEYCODE_D)) {
			camera_.sidestep(amount * deltatime.as_seconds() * camera_speed);
		}

		// Look rotation
		if (mouse_.is_down(MOUSE_BUTTON_RIGHT)) {
			glm::vec2 mouse_delta;
			mouse_delta.x = (float)mouse_.x_;
			mouse_delta.y = (float)mouse_.y_;

			// Calculate mouse movement
			mouse_delta = mouse_delta - mouse_position_;

			if (fabsf(mouse_delta.x) > 0.0f) {
				camera_.rotate_y(glm::radians(mouse_delta.x) * deltatime.as_seconds() * camera_turn_speed);
			}

			if (fabsf(mouse_delta.y) > 0.0f) {
				camera_.rotate_x(glm::radians(mouse_delta.y) * deltatime.as_seconds() * camera_turn_speed);
			}
		}

		// Roll rotation
		if (mouse_.is_down(MOUSE_BUTTON_MIDDLE)) {
			float mouse_delta_x = (float)mouse_.x_ - mouse_position_.x;

			if (fabsf(mouse_delta_x) > 0) {
				camera_.rotate_z(glm::radians(mouse_delta_x) * deltatime.as_seconds() * camera_turn_speed);
			}
		}

		// Store old mouse position to calculate movement delta
		mouse_position_ = glm::vec2((float)mouse_.x_, (float)mouse_.y_);

		camera_.update();
	}

	skybox::skybox()
	{
	}

	bool skybox::create()
	{
		const char* names[] = 
		{
			"assets/skybox/xpos.png",
			"assets/skybox/xneg.png",
			"assets/skybox/ypos.png",
			"assets/skybox/yneg.png",
			"assets/skybox/zpos.png",
			"assets/skybox/zneg.png"
		};

		image sides[6];
		for (int index = 0; index < 6; index++) {
			if (!sides[index].create_from_file(names[index])) {
				assert(!"Could not load cubemap image!");
				return false;
			}
		}

		const int width = sides[0].width();
		const int height = sides[0].height();

		for (int index = 1; index < 6; index++) {
			if (width != sides[index].width() || height != sides[index].height()) {
				assert(!"cubemap images must be same dimension");
				return false;
			}
		}

		// Populate faces with data of images
		const void* faces[6];
		for (int index = 0; index < 6; index++) {
			faces[index] = sides[index].data();
		}

		if (!cubemap_.create_cubemap(width, height, faces)) {
			assert(!"Could not create cubemap");
			return false;
		}

		const float Q = 1.0f;
		const glm::vec3 vertices[] =
		{
			// x positive
			{  Q,  Q, -Q },
			{  Q,  Q,  Q },
			{  Q, -Q,  Q },
			{  Q, -Q,  Q },
			{  Q, -Q, -Q },
			{  Q,  Q, -Q },

			// x negative
			{ -Q,  Q,  Q },
			{ -Q,  Q, -Q },
			{ -Q, -Q, -Q },
			{ -Q, -Q, -Q },
			{ -Q, -Q,  Q },
			{ -Q,  Q,  Q },

			// y positive
			{ -Q,  Q,  Q },
			{  Q,  Q,  Q },
			{  Q,  Q, -Q },
			{  Q,  Q, -Q },
			{ -Q,  Q, -Q },
			{ -Q,  Q,  Q },

			// y negative
			{ -Q, -Q, -Q },
			{  Q, -Q, -Q },
			{  Q, -Q,  Q },
			{  Q, -Q,  Q },
			{ -Q, -Q,  Q },
			{ -Q, -Q, -Q },

			// z positive
			{ -Q,  Q, -Q },
			{  Q,  Q, -Q },
			{  Q, -Q, -Q },
			{  Q, -Q, -Q },
			{ -Q, -Q, -Q },
			{ -Q,  Q, -Q },

			// z negative
			{  Q,  Q,  Q },
			{ -Q,  Q,  Q },
			{ -Q, -Q,  Q },
			{ -Q, -Q,  Q },
			{  Q, -Q,  Q },
			{  Q,  Q,  Q },
		};

		if (!buffer_.create(sizeof(vertices), vertices)) {
			return false;
		}

		format_.add_attribute(0, 3, GL_FLOAT, false);

		if (!program_.create("assets/skybox/vertex_shader.shader", "assets/skybox/fragment_shader.shader")) {
			return false;
		}

		if (!sampler_.create(GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
			return false;
		}

		return true;
	}

	void skybox::destroy()
	{
	}

	void skybox::render(const fps_camera& camera)
	{
		glm::mat4 fixed_view = camera.view_;
		// Reset translation to not move skybox
		fixed_view[3][0] = 0.0f;
		fixed_view[3][1] = 0.0f;
		fixed_view[3][2] = 0.0f;

		program_.bind();
		program_.set_uniform_mat4("projection", camera.projection_);
		program_.set_uniform_mat4("view", fixed_view); // we only want to see camera rotation, not position

		buffer_.bind();
		format_.bind();
		cubemap_.bind();
		sampler_.bind();

		glDisable(GL_DEPTH_TEST);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
} //!neon
