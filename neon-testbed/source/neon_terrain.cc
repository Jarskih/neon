#include "neon_terrain.h"

namespace neon {

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
		const int32 stride = height * channels;

		// Get vertex height (y axis values)
		dynamic_array<vertex> vertices;
		float scale = 0.1f;

		for (int32 x = 0; x < height; x++) {
			for (int32 y = 0; y < width; y++) {
				const uint32 offset = x * channels + y * stride;
				const uint8* rbga = heightmap.data() + offset;

				vertex vertex_;
				vertex_.position_ = { x, rbga[2] * scale, y };

				// calculate uv in range of 0-1
				float u = (float)x / width;
				float v = (float)y / height;
				vertex_.texcoord_ = { u, v };

				// normals
				if (x != 0 && y != 0 && x < width - 1 && y < height - 1) { // Ignore border pixels

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
						offSet = x * channels + (y + 1) * stride;
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
						offSet = (x + 1) * channels + (y + 1) * stride;
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
						offSet = (x - 1) * channels + y * stride;
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
						offSet = (x - 1) * channels + (y - 1) * stride;
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
		for (uint32 index = 0; index < (uint32)width * (uint32)height; index++) {

			if (x >= width - 1) {
				x = 0;
				y++;
				continue;
			}

			x++;

			if (y >= height - 1) {
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

	void terrain::destroy()
	{
	}

	void terrain::render(const fps_camera& camera, const directional_light& light, glm::mat4 world)
	{
		glm::mat4 light_matrix = light.projection_ * world;

		program_.bind();
		program_.set_uniform_mat4("projection", camera.projection_);
		program_.set_uniform_mat4("world", world);
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

	void terrain::render_shadow_map(const directional_light& light)
	{
		glm::mat4 light_matrix = light.projection_ * glm::mat4(1);

		shadow_program_.bind();
		shadow_program_.set_uniform_mat4("lightSpaceMatrix", light_matrix);
		shadow_program_.set_uniform_mat4("model", glm::mat4(1));

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