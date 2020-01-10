// neon_model.h

#ifndef NEON_MODEL_H_INCLUDED
#define NEON_MODEL_H_INCLUDED

#include "neon_graphics.h"

// note: forward declare
struct aiNode;
struct aiMesh;
struct aiScene;

namespace neon {
   struct model {
      struct mesh {
         mesh();
         explicit mesh(int32 start, int32 count);

         int32 start_;
         int32 count_;
      };

      struct vertex {
         glm::vec3 position_;
         glm::vec2 texcoord_;
		// glm::vec3 normal_;
      };

      model();

      bool is_valid() const;
      bool create_from_file(const string &filename, const string& vertex, const string& fragment, const string& diffuse);
      void destroy();

      void render(const fps_camera &camera, const glm::mat4 &world);
      void render_shadow_map(const directional_light& light);

      // note: assimp
      bool process_node(const aiNode *node, const aiScene *scene);
      bool process_mesh(const aiMesh *mesh, const aiScene *scene);

      shader_program program_;
      shader_program shadow_program_;
      texture texture_;
      sampler_state sampler_;
      vertex_buffer vertex_buffer_;
      index_buffer index_buffer_;
      vertex_format vertex_format_;
      dynamic_array<mesh> meshes_;
      dynamic_array<vertex> vertices_;
      dynamic_array<uint32> indices_;
   };
} // !neon

#endif // !NEON_MODEL_H_INCLUDED
