// neon_model.cc

#include "neon_model.h"

// notes: 
// - C/C++ > General > Additional Include Directories (add to end): external\assimp\include\;
// - Linker > General > Additional Library Directories (add to end): external\assimp\lib\;
// - Linker > Input > Additional Dependencies (add to end): assimp-vc142-mtd.lib
// - copy external/assimp/assimp-vc142-mtd.dll to neon/build

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace neon {
   model::mesh::mesh()
      : start_(0)
      , count_(0)
   {
   }

   model::mesh::mesh(int32 start, int32 count)
      : start_(start)
      , count_(count)
   {
   }

   model::model()
   {
   }

   bool model::is_valid() const {
      return !meshes_.empty();
   }

   bool model::create_from_file(const string &filename, const string &vertex, const string &fragment, const string &diffuse) {
      if (!program_.create(vertex, fragment)) {
         return false;
      }

      if (!shadow_program_.create("assets/shadow/vertex.shader", "assets/shadow/fragment.shader")) {
          return false;
      }

      if (!texture_.create(diffuse)) {
         return false;
      }

      if (!sampler_.create(GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
         return false;
      }

      GLint position_location = program_.get_attrib_location("position");
      GLint texcoord_location = program_.get_attrib_location("texcoord");
      vertex_format_.add_attribute(position_location, 3, GL_FLOAT, false);
      vertex_format_.add_attribute(texcoord_location, 2, GL_FLOAT, false);

      // note: assimp-ery
      Assimp::Importer importer;
      const aiScene *scene = importer.ReadFile(filename.c_str(), aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);
      if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) {
         return false;
      }

      if (!process_node(scene->mRootNode, scene)) {
         return false;
      }

       if (!vertex_buffer_.create((int32)(sizeof(vertex) * vertices_.size()), vertices_.data())) {
         return false;
      }
      vertices_.clear();

      if (!index_buffer_.create((int32)(sizeof(uint32) * indices_.size()), GL_UNSIGNED_INT, indices_.data())) {
         return false;
      }
      indices_.clear();

      return true;
   }

   void model::destroy() {
      program_.destroy();
      texture_.destroy();
      sampler_.destroy();
      vertex_buffer_.destroy();
      index_buffer_.destroy();
   }

   void model::render(const fps_camera &camera, const glm::mat4 &world) {
      GLenum err = GL_NO_ERROR;

      glEnable(GL_DEPTH_TEST);
      glDisable(GL_CULL_FACE);
      glFrontFace(GL_CW);

      program_.bind();
      program_.set_uniform_mat4("projection", camera.projection_);
      program_.set_uniform_mat4("view", camera.view_);
      program_.set_uniform_mat4("world", world);

      texture_.bind();
      sampler_.bind();

      vertex_buffer_.bind();
      vertex_format_.bind();
      index_buffer_.bind();

      for (auto &mesh : meshes_) {
         index_buffer_.render(GL_TRIANGLES, mesh.start_, mesh.count_);
      }
   }

   // note: assimp-ery processing
   bool model::process_node(const aiNode *ai_node, const aiScene *ai_scene) {
      for (uint32 index = 0; index < ai_node->mNumMeshes; index++) {
         if (!process_mesh(ai_scene->mMeshes[ai_node->mMeshes[index]], ai_scene)) {
            return false;
         }
      }

      for (uint32 index = 0; index < ai_node->mNumChildren; index++) {
         if (!process_node(ai_node->mChildren[index], ai_scene)) {
            return false;
         }
      }

      return true;
   }

   bool model::process_mesh(const aiMesh *ai_mesh, const aiScene *ai_scene) {
      for (uint32 index = 0; index < ai_mesh->mNumVertices; index++) {
         aiVector3D position = ai_mesh->mVertices[index];
         aiVector3D texcoord;
         if (ai_mesh->HasTextureCoords(0)) {
            texcoord = ai_mesh->mTextureCoords[0][index];
         }
		 aiVector3D normal;
		// if (ai_mesh->HasNormals) {
		//	 normal = ai_mesh->mNormals[index];
		// }

         vertex vert = {};
         vert.position_ = { position.x, position.y, position.z };
         vert.texcoord_ = { texcoord.x, texcoord.y };
		// vert.normal_ = { normal.x, normal.y, normal.z };
         vertices_.push_back(vert);
      }

      const int32 start = (int32)indices_.size();
      for (uint32 face_index = 0; face_index < ai_mesh->mNumFaces; face_index++) {
         const aiFace face = ai_mesh->mFaces[face_index];
         assert(face.mNumIndices == 3);
         for (uint32 index = 0; index < face.mNumIndices; index++) {
            indices_.push_back(face.mIndices[index]);
         }
      }

      const int32 count = (int32)indices_.size() - start;
      meshes_.push_back(mesh(start, count));

      return true;
   }

   void model::render_shadow_map(const directional_light& light)
   {
       GLenum err = GL_NO_ERROR;

       glEnable(GL_DEPTH_TEST);
       glDisable(GL_CULL_FACE);
       glFrontFace(GL_CW);

       glm::mat4 light_matrix = light.projection_ * light.view_;

       shadow_program_.bind();
       shadow_program_.set_uniform_mat4("lightSpaceMatrix", light_matrix);
       shadow_program_.set_uniform_mat4("model", glm::mat4(1));

       texture_.bind();
       sampler_.bind();

       vertex_buffer_.bind();
       vertex_format_.bind();
       index_buffer_.bind();

       for (auto& mesh : meshes_) {
           index_buffer_.render(GL_TRIANGLES, mesh.start_, mesh.count_);
       }
   }

} // !neon
