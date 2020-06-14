#ifndef MAINSCENE
#define MAINSCENE

// MARE
#include "Scene.hpp"
using namespace mare;

// External Libraries
#include "glm.hpp"

// 3DH
#include "Layers/RibbonLayer.hpp"

class MainScene : public Scene {
public:
  MainScene() : Scene(ProjectionType::ORTHOGRAPHIC) {
    gen_layer<RibbonLayer>();
  }
  void on_enter() override {}
  void render(float dt) override { Renderer::clear_color_buffer(bg_color); }
  void on_exit() override {}

private:
  glm::vec4 bg_color = {0.8f, 0.8f, 0.9f, 1.0f};
};

#endif