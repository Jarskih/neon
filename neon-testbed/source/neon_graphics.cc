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

	bool bitmap_font::create()
	{
		if (!program_.create("assets/bitmap_font_vertex_shader.shader", "assets/bitmap_font_fragment_shader.shader")) {
			return false;
		}

		format_.add_attribute(0, 2, GL_FLOAT, false);
		format_.add_attribute(1, 2, GL_FLOAT, false);

		if (!buffer_.create(512, nullptr)) {
			return false;
		}

		if (!texture_.create("assets/font_8x8.png", false)) {
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

	directional_light::directional_light() : color_(0), projection_(1), view_(1), direction_(0)
	{
	}

	bool directional_light::create(glm::vec4 color, glm::mat4 projection, glm::vec3 direction) {
		color_ = color;
		projection_ = projection;
		direction_ = direction;
		view_ = glm::lookAt(-direction_, glm::vec3(0), glm::vec3(0, -1, 0));
		return true;
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

	terrain::terrain() : index_count_(0), position_(0)
	{
	}

	bool terrain::create(const string& heightmap_filemap, const string& texture_filename)
	{
		image heightmap;
		if (!heightmap.create_from_file(heightmap_filemap.c_str())) {
			return false;
		}

		const int32 width = heightmap.width();
		const int32 height = heightmap.height();
		const int32 channels = 4; // note: 4 channels per pixel
		const int32 stride = height* channels; 

		// Get vertex height (y axis values)
		dynamic_array<vertex> vertices;
		float scale = 0.1f;

		for (int32 x = 0; x < height; x++) {
			for (int32 y = 0; y < width; y++) {
				const uint32 offset = x * channels + y * stride;
				const uint8 *rbga = heightmap.data() + offset;
 
				vertex vertex_;
				vertex_.position_ = { x, rbga[2] * scale, y };

				// calculate uv in range of 0-1
				float u = (float)x / width;
				float v = (float)y / height;
				vertex_.texcoord_ = { u, v };

				// normals
				if(x != 0 && y != 0 && x < width-1 && y < height-1) { // Ignore border pixels

					glm::vec3 normal;

					// +-- + -- +
					// | \ | \ |
					// +-- + -- +
					// | \ | \ |
					// +-- + -- +

					// first triangle
					{
						uint32 offSet = 0;
						uint8* vertex_rbga = nullptr;

						vertex v0;
						v0.position_ = { x, rbga[2] * scale, y };

						vertex v1;
						offSet = x * channels + (y - 1) * stride;
						vertex_rbga = heightmap.data() + offSet;
						v1.position_ = { x, vertex_rbga[2] * scale, y - 1 };

						vertex v2;
						offSet = (x + 1) * channels + y * stride;
						vertex_rbga = heightmap.data() + offSet;
						v2.position_ = { x + 1, vertex_rbga[2] * scale, y };

						const glm::vec3 side1 = v1.position_ - v0.position_;
						const glm::vec3 side2 = v2.position_ - v1.position_;
						const glm::vec3 cross = glm::cross(side1, side2);
						normal += cross;
					}

					// second triangle
					{
						uint32 offSet = 0;
						uint8* vertex_rbga = nullptr;

						vertex v0;
						v0.position_ = { x, rbga[2] * scale, y };

						vertex v1;
						offSet = (x + 1) * channels + y * stride;
						vertex_rbga = heightmap.data() + offset;
						v1.position_ = { x + 1, vertex_rbga[2] * scale, y };

						vertex v2;
						offSet = x * channels + (y+1) * stride;
						vertex_rbga = heightmap.data() + offset;
						v2.position_ = { x, vertex_rbga[2] * scale, y + 1 };

						const glm::vec3 side1 = v1.position_ - v0.position_;
						const glm::vec3 side2 = v2.position_ - v1.position_;
						const glm::vec3 cross = glm::cross(side1, side2);
						normal += cross;
					}

					// third triangle
					{
						uint32 offSet = 0;
						uint8* vertex_rbga = nullptr;

						vertex v0;
						v0.position_ = { x, rbga[2] * scale, y };

						vertex v1;
						offSet = (x + 1) * channels + (y+1) * stride;
						vertex_rbga = heightmap.data() + offset;
						v1.position_ = { x + 1, vertex_rbga[2] * scale, y + 1 };

						vertex v2;
						offSet = x * channels + (y + 1) * stride;
						vertex_rbga = heightmap.data() + offset;
						v2.position_ = { x, vertex_rbga[2] * scale, y + 1 };

						const glm::vec3 side1 = v1.position_ - v0.position_;
						const glm::vec3 side2 = v2.position_ - v1.position_;
						const glm::vec3 cross = glm::cross(side1, side2);
						normal += cross;
					}

					// fourth triangle
					{
						uint32 offSet = 0;
						uint8* vertex_rbga = nullptr;

						vertex v0;
						v0.position_ = { x, rbga[2] * scale, y };

						vertex v1;
						offSet = x * channels + (y + 1) * stride;
						vertex_rbga = heightmap.data() + offset;
						v1.position_ = { x, vertex_rbga[2] * scale, y + 1 };

						vertex v2;
						offSet = (x-1) * channels + y * stride;
						vertex_rbga = heightmap.data() + offset;
						v2.position_ = { x - 1, vertex_rbga[2] * scale, y };

						const glm::vec3 side1 = v1.position_ - v0.position_;
						const glm::vec3 side2 = v2.position_ - v1.position_;
						const glm::vec3 cross = glm::cross(side1, side2);
						normal += cross;
					}

					// fifth triangle
					{
						uint32 offSet = 0;
						uint8* vertex_rbga = nullptr;

						vertex v0;
						v0.position_ = { x, rbga[2] * scale, y };

						vertex v1;
						offSet = (x - 1) * channels + y * stride;
						vertex_rbga = heightmap.data() + offset;
						v1.position_ = { x - 1, vertex_rbga[2] * scale, y };

						vertex v2;
						offSet = (x - 1) * channels + (y-1) * stride;
						vertex_rbga = heightmap.data() + offset;
						v2.position_ = { x - 1, vertex_rbga[2] * scale, y - 1 };

						const glm::vec3 side1 = v1.position_ - v0.position_;
						const glm::vec3 side2 = v2.position_ - v1.position_;
						const glm::vec3 cross = glm::cross(side1, side2);
						normal += cross;
					}

					// sixth triangle
					{
						uint32 offSet = 0;
						uint8* vertex_rbga = nullptr;

						vertex v0;
						v0.position_ = { x , rbga[2] * scale, y };

						vertex v1;
						offSet = (x - 1) * channels + (y - 1) * stride;
						vertex_rbga = heightmap.data() + offset;
						v1.position_ = { x - 1, vertex_rbga[2] * scale, y - 1 };

						vertex v2;
						offSet = x * channels + (y - 1) * stride;
						vertex_rbga = heightmap.data() + offset;
						v2.position_ = { x, vertex_rbga[2] * scale, y - 1 };

						const glm::vec3 side1 = v1.position_ - v0.position_;
						const glm::vec3 side2 = v2.position_ - v1.position_;
						const glm::vec3 cross = glm::cross(side1, side2);
						normal += cross;
					}

					vertex_.normal_ = glm::normalize(normal);
				}

				vertices.push_back(vertex_);
			}
		}

		if (!vertex_buffer_.create(sizeof(vertex) * (int)vertices.size(), vertices.data())) {
			return false;
		}
		
		dynamic_array<uint32> index_array;
		int x = 0; // pic width
		int y = 0; // pic height
		uint32 offset = 1;
		for (uint32 index = 0; index < (uint32)width * (uint32)height; index ++) {

			if (x >= width-1) {
				x = 0;
				y++;
				continue;
			}

			x++;

			if (y >= height-1) {
				break;
			}

			// First triangle
	
			index_array.push_back(index);
			index_array.push_back(index + offset);
			index_array.push_back(index + offset + width);

			// Second triangle
			index_array.push_back(index + offset + width);
			index_array.push_back(index + width);
			index_array.push_back(index);
		}

		if (!index_buffer_.create(sizeof(int) * (int)index_array.size(), GL_UNSIGNED_INT, index_array.data())) {
			return false;
		}

		index_count_ = (int)index_array.size();
		format_.add_attribute(0, 3, GL_FLOAT, false);
		format_.add_attribute(1, 2, GL_FLOAT, false);
		format_.add_attribute(2, 3, GL_FLOAT, false);

		if (!program_.create("assets/heightmap/vertex_shader.shader", "assets/heightmap/fragment_shader.shader")) {
			return false;
		}

		if (!sampler_.create(GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
			return false;
		}

		if (!texture_.create(texture_filename, false)) {
			return false;
		}
	
		return true;
	}

	void terrain::destroy()
	{
	}

	void terrain::render(const fps_camera& camera, const directional_light& light, shader_program &program) {

		glm::mat4 light_matrix = light.projection_ * light.view_ * glm::mat4(1);

		unsigned int depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);

		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

		unsigned int depthMap;

		glm::mat4 transform = glm::mat4(1.0);
		transform = glm::translate(transform, glm::vec3(0));

		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);

		program.bind();
		program.set_uniform_mat4("projection", light.projection_);
		program.set_uniform_mat4("view", light.view_);
		program.set_uniform_mat4("world", glm::mat4(1));
		program.set_uniform_vec3("light_direction", light.direction_);

		vertex_buffer_.bind();
		index_buffer_.bind();
		format_.bind();
		texture_.bind();
		sampler_.bind();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);

		index_buffer_.render(GL_TRIANGLES, 0, index_count_);

		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void terrain::render(const fps_camera& camera, const directional_light& light)
	{
		glm::mat4 light_matrix = light.projection_ * light.view_ * glm::mat4(1);

		glm::mat4 transform = glm::mat4(1.0);
		transform = glm::translate(transform, position_);

		program_.bind();
		program_.set_uniform_mat4("projection", camera.projection_);
		program_.set_uniform_mat4("view", camera.view_);
		program_.set_uniform_mat4("world", transform);
		program_.set_uniform_mat4("light_matrix", light_matrix);
		program_.set_uniform_vec3("light_direction", light.direction_);
		program_.set_uniform_vec4("light_color", light.color_);

		glm::mat4 conversionMatrix = glm::inverse(camera.view_);
		glm::vec3 cameraPos = (glm::vec3) conversionMatrix[3];	//Get the camera position from the view matrix.
		program_.set_uniform_vec3("camera_pos", cameraPos);

		vertex_buffer_.bind();
		index_buffer_.bind();
		format_.bind();
		texture_.bind();
		sampler_.bind();

		// Culling
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);

		index_buffer_.render(GL_TRIANGLES, 0, index_count_);
	}


	sphere::sphere() : position_(0), radius_(0), stacks_(0), sectors_(0), sectorStep_(0), stackStep_(0), index_count_(0), rotation_(0), spin_(0), rotationSpeed_(0.1f), pivot_(0), isMoon_(false)
	{
	}

	bool sphere::create(std::string texture_filename, float radius, int stacks, int sectors) {
		
		radius_ = radius;
		stacks_ = stacks;
		sectors_ = sectors;
		constexpr float PI = 3.14159265359f;

		vertices_.clear();

		sectorStep_ = 2.0f * PI / sectors;
		stackStep_ = PI / stacks;

		// Calculate vertex positions on the sphere
		for (int stack = 0; stack <= stacks_; ++stack) {

			float stackAngle = PI / 2 - stack * stackStep_;
			float xy = radius_ * cosf(stackAngle);
			float z = radius_ * sinf(stackAngle);

			float lengthInv = 1.0f / radius_;

			// add (sectorCount +1) vertices per stack

			for (int sector = 0; sector <= sectors_; ++sector) {
				float sectorAngle = sector * sectorStep_;

				vertex vertex;

				// Vertex positions
				float x = xy * cosf(sectorAngle);
				float y = xy * sinf(sectorAngle);

				vertex.position_ = { x, y, z };

				// Normalized vertex normals 
				float nx = x * lengthInv;
				float ny = y * lengthInv;
				float nz = z * lengthInv;
				vertex.normal_ = { nx, ny, nz };

				// text coords (range: 0 - 1)
				float u = (float)sector / sectors_;
				float v = (float)stack / stacks_;
				vertex.texcoord_ = { u, v };

				vertices_.push_back(vertex);
			}
		}

		if (!vertex_buffer_.create(sizeof(vertex) * (int)vertices_.size(), vertices_.data())) {
			return false;
		}

		neon::dynamic_array<int> index_array;

		int v1, v2;

		for (int y = 0; y < sectors_; ++y) {

			v1 = y * (sectors_ + 1); // beginning of the current stack
			v2 = v1 + sectors_ + 1;  // beginning of next stack

			for (int x = 0; x < sectors_; ++x, ++v1, ++v2) {
				// 2 triangles per sector excluding first and last stacks
				// v1 => v2 => v1+1

				if (y != 0) {
					index_array.push_back(v1);
					index_array.push_back(v2);
					index_array.push_back(v1 + 1);
				}

				// v1+1 => v2 => v2+1
				if (y != (stacks_ - 1)) {
					index_array.push_back(v1 + 1);
					index_array.push_back(v2);
					index_array.push_back(v2 + 1);
				}
			}
		}

		if (!index_buffer_.create(sizeof(int) * (int)index_array.size(), GL_UNSIGNED_INT, index_array.data())) {
			return false;
		}

		index_count_ = (int)index_array.size();
		format_.add_attribute(0, 3, GL_FLOAT, false);
		format_.add_attribute(1, 2, GL_FLOAT, false);
		format_.add_attribute(2, 3, GL_FLOAT, false);

		if (!program_.create("assets/sphere/vertex_shader.shader", "assets/sphere/fragment_shader.shader")) {
			return false;
		}

		if (!sampler_.create(GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
			return false;
		}

		if (!texture_.create(texture_filename, false)) {
			return false;
		}

		return true;
	}

	void sphere::render(neon::fps_camera camera, const time& dt)
	{
		// rotation
		rotation_ += dt.as_seconds() * rotationSpeed_;
		spin_ += dt.as_seconds();

		glm::mat4 transform = glm::mat4(1.0);

		if (isMoon_) {
			// Rotate around origin (sun)
			transform = glm::rotate(transform, rotation_, glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::translate(transform, pivot_);
			// Rotate around earth
			transform = glm::rotate(transform, spin_, glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::translate(transform, position_);
			// Rotate around self
		    // transform = glm::rotate(transform, spin_, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else {
			// Rotate around origin (sun)
			transform = glm::rotate(transform, rotation_, glm::vec3(0.0f, 0.0f, 1.0f));

			// Translate to orbit
			transform = glm::translate(transform, position_);

			// Rotate around self
			//transform = glm::rotate(transform, spin_, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		program_.bind();
		program_.set_uniform_mat4("projection", camera.projection_);
		program_.set_uniform_mat4("view", camera.view_);
		program_.set_uniform_mat4("world", transform);
		program_.set_uniform_vec3("light_direction", glm::vec3(0, -1, 0));
		program_.set_uniform_vec3("light_pos", glm::vec3(0, 50, 0));
		program_.set_uniform_vec4("light_color", glm::vec4(1,1,0.5f,1));

		glm::mat4 conversionMatrix = glm::inverse(camera.view_);
		glm::vec3 cameraPos = (glm::vec3) conversionMatrix[3];	//Get the camera position from the view matrix.
		program_.set_uniform_vec3("camera_pos", cameraPos);

		vertex_buffer_.bind();
		index_buffer_.bind();
		format_.bind();
		texture_.bind();
		sampler_.bind();

		// Culling
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);

		index_buffer_.render(GL_TRIANGLES, 0, index_count_);
	}

	depth_buffer::depth_buffer() : id_(0)
	{
	}

	bool depth_buffer::create(int width, int height)
	{
		if (is_valid()) {
			return false;
		}

		glGenRenderbuffers(1, &id_);
		glBindRenderbuffer(GL_RENDERBUFFER, id_);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id_);

		GLenum error = glGetError();
		return error == GL_NO_ERROR;
	}

	void depth_buffer::destroy()
	{
	}

	bool depth_buffer::is_valid() const
	{
		return id_ != 0;
	}

	frame_buffer::frame_buffer() : id_(0)
	{
	}

	bool frame_buffer::create(int width, int height)
	{
		if (is_valid()) {
			return false;
		}

		// create framebuffer
		glGenRenderbuffers(1, &id_);
		glBindRenderbuffer(GL_RENDERBUFFER, id_);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id_);

		// create texture attachment
		depth_texture_.createDepthTexture(width, height);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			return false;
		}

		GLenum error = glGetError();
		return error == GL_NO_ERROR;
	}

	void frame_buffer::destroy()
	{
		glDeleteFramebuffers(1, &id_);
		glDeleteTextures(1, &color_texture_.id_);
		glDeleteTextures(1, &depth_texture_.id_);
		//glDeleteRenderbuffers(1, &depth_buffer_.id_);
	}

	void frame_buffer::bind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0); // unbind texture if there is any
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id_);
	}

	void frame_buffer::unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_NONE);
	}

	bool frame_buffer::is_valid() const
	{
		return id_ != 0;
	}
} //!neon
