#ifndef RIBBONLAYER
#define RIBBONLAYER

// MARE
#include "Layer.hpp"
using namespace mare;

// 3DH
#include "Entities/DebugInfo.hpp"
#include "Entities/UI/RibbonUI.hpp"
#include "Entities/RibbonTools/LoadTool.hpp"

/**
 * @brief A Layer used to render the RibbonUI
 *
 */
class RibbonLayer : public Layer {
public:
  /**
   * @brief Construct a new RibbonLayer
   *
   */
  RibbonLayer() : Layer(ProjectionType::ORTHOGRAPHIC) {
    uint32_t ribbon_width_in_pixels = 200;
    auto tool1 = gen_entity<LoadTool>(this, ribbon_width_in_pixels);
    auto tool2 = gen_entity<LoadTool>(this, ribbon_width_in_pixels);
    auto tool3 = gen_entity<LoadTool>(this, ribbon_width_in_pixels);
    auto tool4 = gen_entity<LoadTool>(this, ribbon_width_in_pixels);
    auto ribbon_ui = gen_entity<RibbonUI>(this, ribbon_width_in_pixels);
    ribbon_ui->push_tool(tool1);
    ribbon_ui->push_tool(tool2);
    ribbon_ui->push_tool(tool3);
    ribbon_ui->push_tool(tool4);

    dinfo = gen_entity<DebugInfo>(1000);
    dinfo->color->set_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    dinfo->set_scale(glm::vec3(0.1f));
    std::string debug_str = Renderer::get_vendor_string() + '\n' +
                            Renderer::get_version_string() + '\n' +
                            Renderer::get_renderer_string();
    dinfo->text->set_text(debug_str);
    dinfo->text->set_center(glm::vec3(0.0f));
  }
  void on_enter() override {}
  void render(float dt) override {
    Renderer::enable_depth_testing(false);
    Renderer::enable_blending(true);
    t += dt;
    fps_accum += 1.0 / dt;
    fps_count += 1.0;
    if (t > 0.5f) {
      t = 0.0f;
      dinfo->text->set_text(std::to_string(fps_accum / fps_count));
      dinfo->text->set_center(glm::vec3(0.0f));
      fps_count = 0.0;
      fps_accum = 0.0;
    }
  }
  void on_exit() override {}
  Referenced<DebugInfo> dinfo;
  float t = 0.0f;
  double fps_accum;
  double fps_count;
};

#endif