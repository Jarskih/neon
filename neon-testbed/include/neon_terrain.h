#ifndef NEON_TERRAIN_H_INCLUDED
#define NEON_TERRAIN_H_INCLUDED

#include "neon_graphics.h"

namespace neon {

	struct terrain {

		struct vertex {
			glm::vec3 position_;
			glm::vec2 texcoord_;
			glm::vec3 normal_;
		};

		terrain();

		bool create(const string& heightmap_filemap, const string& texture_filename);
		void destroy();

		void render(const fps_camera& camera, const directional_light& light, glm::mat4 world = glm::mat4(1));
		void render_shadow_map(const directional_light& light, fps_camera camera);

		shader_program program_;
		shader_program shadow_program_;
		vertex_buffer vertex_buffer_;
		vertex_format format_;
		index_buffer index_buffer_;
		texture texture_;
		sampler_state sampler_;
		int index_count_;
		glm::vec3 position_;
	};
}
#endif // !NEON_TERRAIN_H_INCLUDED