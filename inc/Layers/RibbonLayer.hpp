#ifndef RIBBONLAYER
#define RIBBONLAYER

// MARE
#include "Layer.hpp"
using namespace mare;

// 3DH
#include "Entities/DebugInfo.hpp"
#include "Entities/UI/RibbonUI.hpp"

class RibbonLayer : public Layer {
public:
  RibbonLayer() : Layer(ProjectionType::ORTHOGRAPHIC) {
    dinfo = gen_entity<DebugInfo>(1000);
    gen_entity<RibbonUI>(this);
    dinfo->color->set_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    dinfo->set_scale(glm::vec3(0.1f));
    std::string debug_str = Renderer::get_vendor_string() + '\n' + Renderer::get_version_string() + '\n' + Renderer::get_renderer_string();
    dinfo->text->set_text(debug_str);
    dinfo->text->set_center(glm::vec3(0.0f));
  }
  void on_enter() override {}
  void render(float dt) override {
    Renderer::enable_depth_testing(false);
    Renderer::enable_blending(true);
  }
  void on_exit() override {}
  Referenced<DebugInfo> dinfo;
};

#endif