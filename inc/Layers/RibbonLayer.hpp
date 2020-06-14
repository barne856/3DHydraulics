#ifndef RIBBONLAYER
#define RIBBONLAYER

// MARE
#include "Layer.hpp"
using namespace mare;

// 3DH
#include "Entities/UI/RibbonUI.hpp"

class RibbonLayer : public Layer {
public:
  RibbonLayer() : Layer(ProjectionType::ORTHOGRAPHIC) {
      gen_entity<RibbonUI>(this);
  }
  void on_enter() override {}
  void render(float dt) override {
    Renderer::enable_depth_testing(false);
    Renderer::enable_blending(true);
  }
  void on_exit() override {}
};

#endif