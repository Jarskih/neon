#include "neon_sphere.h"

namespace neon {
	sphere::sphere() : radius_(0), stacks_(0), sectors_(0), sectorStep_(0), stackStep_(0), index_count_(0), rotation_(0), spin_(0), rotationSpeed_(0.1f), pivot_(0), isMoon_(false)
	{
	}

	constexpr float PI = 3.14159265359f;

	bool sphere::create(std::string texture_filename, float radius, int stacks, int sectors, float orbit) {

		radius_ = radius;
		stacks_ = stacks;
		sectors_ = sectors;
		orbit_ = orbit;
		rotationSpeed_ = 10.0f;

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

		if (!shadow_program_.create("assets/shadow/vertex.shader", "assets/shadow/fragment.shader")) {
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
		float angle = 2 * PI / orbit_;
		rotation_ += dt.as_seconds() * angle * rotationSpeed_;
		spin_ += dt.as_seconds();

		glm::mat4 transform = glm::mat4(1.0);

		// Rotate around origin (sun)
		transform = glm::rotate(transform, rotation_, glm::vec3(0.0f, 0.0f, 1.0f));

		// Translate to orbit
		transform = glm::translate(transform, position_);

		// Rotate around self
		transform = glm::rotate(transform, spin_, glm::vec3(0.0f, 0.0f, 1.0f));

		program_.bind();
		program_.set_uniform_mat4("view", camera.view_);
		program_.set_uniform_mat4("projection", camera.projection_);
		program_.set_uniform_mat4("world", transform);

		program_.set_uniform_vec3("light_direction", glm::vec3(0, -1, 0));
		program_.set_uniform_vec4("light_color", glm::vec4(1, 1, 0.5f, 1));

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
	void sphere::render(const fps_camera& camera, const time& dt, const directional_light& light, glm::mat4 world)
	{
		// rotation
		float angle = 2 * PI / orbit_;
		rotation_ += dt.as_seconds() * angle * rotationSpeed_;
		spin_ += dt.as_seconds();

		glm::mat4 transform = glm::mat4(1.0);

		// Rotate around origin
		transform = glm::rotate(transform, rotation_, glm::vec3(0.0f, 0.0f, 1.0f));

		// Translate to orbit
		transform = glm::translate(transform, position_);

		glm::vec3 pos = glm::vec3(0);
		glm::mat4 view_ = glm::lookAt(pos, light.direction_, glm::vec3(0, 1, 0));
		glm::mat4 lightMV = light.projection_ * view_;

		program_.bind();
		program_.set_uniform_mat4("view", camera.view_);
		program_.set_uniform_mat4("world", transform);
		program_.set_uniform_mat4("projection", camera.projection_);

		program_.set_uniform_vec3("light_direction", light.direction_);
		program_.set_uniform_vec4("light_color", light.color_);
		program_.set_uniform_mat4("light_matrix", lightMV);

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

	void sphere::render_shadow_map(const directional_light& light, fps_camera camera)
	{
		glm::vec3 pos = glm::vec3(0);
		glm::mat4 view_ = glm::lookAt(pos, light.direction_, glm::vec3(0, 1, 0));
		glm::mat4 lightMV = light.projection_ * view_;

		shadow_program_.bind();
		shadow_program_.set_uniform_mat4("light_matrix", lightMV);
		shadow_program_.set_uniform_mat4("world", glm::mat4(1));

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
}