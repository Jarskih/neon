// neon_graphics.h

#include "neon_graphics.h"

namespace neon {
struct bounding_sphere {
   bounding_sphere();
   explicit bounding_sphere(const glm::vec3 &center, float radius);

   void set_center(const glm::vec3 &center);
   void set_radius(const float radius);

   glm::vec3 center_;
   float radius_;
};

struct plane {
   enum plane_type_id {
      PLANE_NEAR,
      PLANE_FAR,
      PLANE_LEFT,
      PLANE_RIGHT,
      PLANE_TOP,
      PLANE_BOTTOM,
      PLANE_COUNT,
   };

   plane();

   glm::vec3 normal_;
   float d_;
};

struct frustum {
   frustum();

   void construct_from_view_matrix(const glm::mat4 &view);

   bool is_inside(const glm::vec3 &point) const;
   bool is_inside(const bounding_sphere &sphere) const;

   plane planes_[plane::PLANE_COUNT];
};

struct transform {
   transform();

   void set_origin(const glm::vec3 &origin);
   void set_position(const glm::vec3 &position);
   void set_scale(const glm::vec3 &scale);
   void set_rotation(const glm::vec3 &rotation);

   glm::vec3 origin_;
   glm::vec3 position_;
   glm::vec3 scale_;
   glm::vec3 rotation_;
};

struct renderable {
   renderable();

   void render(const fps_camera &camera);

   shader_program *program_;
   texture *texture_;
   sampler_state *sampler_state_;
   vertex_buffer *vertex_buffer_;
   index_buffer *index_buffer_;
   vertex_format *format_;
};

struct scene {
   struct node {
      node();

      renderable renderable_;
      transform transform_;
      bounding_sphere sphere_;
   };

   scene();

   void render(const fps_camera &camera);

   dynamic_array<node> nodes_;
}; 
} // !neon


// neon_graphics.cc

namespace neon {
   bounding_sphere::bounding_sphere()
      : center_{}
      , radius_{}
   {
   }

   bounding_sphere::bounding_sphere(const glm::vec3 &center, float radius)
      : center_(center)
      , radius_(radius)
   {
   }

   void bounding_sphere::set_center(const glm::vec3 &center) {
      center_ = center;
   }

   void bounding_sphere::set_radius(const float radius) {
      radius_ = radius;
   }

   plane::plane()
      : normal_{}
      , d_{}
   {
   }

   frustum::frustum()
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

   bool frustum::is_inside(const glm::vec3 &point) const {
      for (int32 index = 0; index < plane::PLANE_COUNT; index++) {
         float dist = glm::dot(planes_[index].normal_, point) + 
                      planes_[index].d_;
         if (dist < 0.0f)
            return false;
      }

      return true;
   }

   bool frustum::is_inside(const bounding_sphere &sphere) const {
      for (int32 index = 0; index < plane::PLANE_COUNT; index++) {
         float dist = glm::dot(planes_[index].normal_, sphere.center_) + 
                      planes_[index].d_;
         if (dist < -sphere.radius_)
            return false;
      }

      return true;
   }

   transform::transform()
      : origin_(0.0f, 0.0f, 0.0f)
      , position_(0.0f, 0.0f, 0.0f)
      , scale_(1.0f, 1.0f, 1.0f)
      , rotation_(0.0f, 0.0f, 0.0f)
   {
   }

   void transform::set_origin(const glm::vec3 &origin) {
      origin_ = origin;
   }

   void transform::set_position(const glm::vec3 &position) {
      position_ = position;
   }

   void transform::set_scale(const glm::vec3 &scale) {
      scale_ = scale;
   }

   void transform::set_rotation(const glm::vec3 &rotation) {
      rotation_ = rotation;
   }

   renderable::renderable()
      : program_(nullptr)
      , texture_(nullptr)
      , sampler_state_(nullptr)
      , vertex_buffer_(nullptr)
      , index_buffer_(nullptr)
      , format_(nullptr)
   {
   }

   void renderable::render(const fps_camera &camera) {
      // ...
   }

   scene::node::node()
   {
   }

   scene::scene()
   {
   }

   void scene::render(const fps_camera &camera) {
      frustum frustum;
      frustum.construct_from_view_matrix(camera.view_);

      for (auto &&node : nodes_) {
         if (frustum.is_inside(node.sphere_)) {
            node.renderable_.render(camera);
         }
      }
   }
} // !neon