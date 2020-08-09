#ifndef MAINSCENE
#define MAINSCENE

// MARE
#include "Components/Rigidbody.hpp"
#include "Scene.hpp"
#include "Systems/Controls/FlyControls.hpp"
#include "Systems/Controls/OrbitControls.hpp"

using namespace mare;

// External Libraries
#include "glm.hpp"

// Standard Library
#include <cmath>

// 3DH
#include "Entities/GridHelper.hpp"
#include "Entities/HydraulicNetwork.hpp"
#include "Layers/RibbonLayer.hpp"

class MainCameraPhysics;

class MainScene : public Scene, public Rigidbody {
public:
  MainScene()
      : Scene(ProjectionType::ORTHOGRAPHIC),
        world_offset(glm::vec3(std::nan(""))) {
    gen_layer<RibbonLayer>();
    HydraulicNetwork::LoadedNetwork = gen_ref<HydraulicNetwork>();
    push_entity(HydraulicNetwork::LoadedNetwork);
    set_far_clip_plane_persp(100000.0f);
    set_near_clip_plane_persp(10.0f);
    set_far_clip_plane_ortho(100000.0f);
    set_near_clip_plane_ortho(100.0f);
    float distance_to_center = 5000.0f;
    set_ortho_scale(distance_to_center *
                               tanf(math::PI / 8.0f));
    set_position({0.0f, 0.0f, distance_to_center});
    auto sys = gen_system<OrbitControls>();
    sys->distance_to_center = distance_to_center;
    sys->is_2D = true;
    grid_helper = gen_ref<GridHelper>(25, 25);
    push_entity(grid_helper);
    grid_helper->set_scale(glm::vec3(15000.0f));
  }
  ~MainScene() { HydraulicNetwork::LoadedNetwork = nullptr; }
  void on_enter() override {}
  void render(float dt) override {
    Renderer::enable_depth_testing(true);
    Renderer::clear_depth_buffer();
    Renderer::enable_blending(true);
    Renderer::clear_color_buffer(bg_color);
    // Renderer::wireframe_mode(true);
  }
  void on_exit() override {}
  inline glm::dvec3 get_world_offset() { return world_offset; }
  inline void set_world_offset(glm::dvec3 offset) { world_offset = offset; }

private:
  glm::vec4 bg_color = {45.0f / 255.0f, 45.0f / 255.0f, 45.0f / 255.0f, 1.0f};
  glm::dvec3 world_offset;
  Referenced<GridHelper> grid_helper;
};

class MainCameraPhysics : public PhysicsSystem<MainScene> {
public:
  void update(float dt, MainScene *scene) override {
    // if there is a terrain, set center elevation of orbit to terrain elevaiton
    // at center position.
    if (auto terra = scene->get_entity<Terrain>()) {
      if (auto controls = scene->get_system<OrbitControls>()) {
        glm::vec3 dir = scene->get_forward_vector();
        glm::vec3 pos = scene->get_position();
        float dist_to_center = controls->distance_to_center;
        glm::vec3 center = pos + dir * dist_to_center;
        float terrain_elevation = terra->get_terrain_elevation(center);
        if (std::isnan(terrain_elevation)) {
          // outside of terrain bounds
          return;
        }
        float center_elevation = (terrain_elevation * terra->get_vert_exag()) -
                                 scene->get_world_offset().z;
        float delta_z = center_elevation - center.z;
        lerp_position = pos;
        lerp_position.z += delta_z;

        glm::vec3 force =
            k * (lerp_position - pos) - c * scene->linear_velocity;
        scene->linear_velocity += force * dt;
        scene->translate(scene->linear_velocity * dt);
      }
    }
  }
  glm::vec3 lerp_position;
  // Critically damped harmonic oscillator, mass = 1.
  const float k = 1000.0f; // strength of the spring.
  const float c =
      2.0f * sqrtf(k); // strength of the damping (critically damped).
};

#endif