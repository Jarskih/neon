#ifndef NEON_SPHERE_H_INCLUDED
#define NEON_SPHERE_H_INCLUDED

#include "neon_graphics.h"

namespace neon {
	struct sphere {

		struct vertex {
			glm::vec3 position_;
			glm::vec2 texcoord_;
			glm::vec3 normal_;
		};
		
		sphere();

		bool create(std::string texture_filename, float radius, int stacks, int sectors, float orbit);
		void render(fps_camera camera, const time& dt);

		void render(const fps_camera& camera, const time& dt, const directional_light& light, glm::mat4 world = glm::mat4(1));
		void render_shadow_map(const directional_light& light, fps_camera camera);

		glm::vec3 position_;
		float radius_;
		int stacks_;
		int sectors_;
		float sectorStep_;
		float stackStep_;
		int index_count_;
		float rotation_;
		float spin_;
		float rotationSpeed_;
		glm::vec3 pivot_;
		bool isMoon_;
		float orbit_;

		dynamic_array<vertex> vertices_;
		shader_program program_;
		shader_program shadow_program_;
		vertex_buffer vertex_buffer_;
		vertex_format format_;
		index_buffer index_buffer_;
		texture texture_;
		sampler_state sampler_;
	};

}

#endif
