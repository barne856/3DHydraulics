#ifndef MAINSCENE
#define MAINSCENE

// MARE
#include "Scene.hpp"
#include "Systems/Controls/FlyControls.hpp"
#include "Systems/Controls/OrbitControls.hpp"
using namespace mare;

// External Libraries
#include "glm.hpp"

// 3DH
#include "Entities/HydraulicNetwork.hpp"
#include "Layers/RibbonLayer.hpp"
#include "Entities/Terrain.hpp"

class MainScene : public Scene {
public:
  MainScene() : Scene(ProjectionType::PERSPECTIVE) {
    gen_layer<RibbonLayer>();
    HydraulicNetwork::LoadedNetwork = gen_ref<HydraulicNetwork>();
    push_entity(HydraulicNetwork::LoadedNetwork);
    auto terrain = gen_entity<Terrain>(5, 5);
    terrain->set_scale(glm::vec3(5000.0f));
    set_far_clip_plane_persp(100000.0f);
    set_near_clip_plane_persp(100.0f);
    set_far_clip_plane_ortho(100000.0f);
    set_near_clip_plane_ortho(100.0f);
    set_ortho_scale(5000.0f);
    set_position({0.0f, 0.0f, 5000.0f});
    //face_towards(glm::vec3(0.0f), {0.0f, 0.0f, 1.0f});
    auto sys = gen_system<OrbitControls>();
    sys->distance_to_center = 5000.0f;
    sys->zoom_sensitvity = 100.0f;
  }
  ~MainScene() { HydraulicNetwork::LoadedNetwork = nullptr; }
  void on_enter() override {}
  void render(float dt) override {
    Renderer::enable_depth_testing(true);
    Renderer::clear_depth_buffer();
    Renderer::enable_blending(true);
    Renderer::clear_color_buffer(bg_color);
  }
  void on_exit() override {}

private:
  glm::vec4 bg_color = {0.8f, 0.8f, 0.9f, 1.0f};
};

#endif