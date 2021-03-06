#ifndef RIBBONLAYER
#define RIBBONLAYER

// MARE
#include "Entities/UI/Dropdown.hpp"
#include "Layer.hpp"

using namespace mare;

// 3DH
#include "Entities/DebugInfo.hpp"
#include "Entities/RibbonTools/LoadTool.hpp"
#include "Entities/RibbonTools/NodeTool.hpp"
#include "Entities/RibbonTools/NavigationTools.hpp"
#include "Entities/RibbonTools/TerrainTool.hpp"
#include "Entities/UI/RibbonUI.hpp"

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
    // auto load_tool = gen_entity<LoadTool>(this, ribbon_width_in_pixels);
    // auto terrain_tool = gen_entity<TerrainTool>(this,
    // ribbon_width_in_pixels);
    auto orbit_tool = gen_entity<OrbitTool>(this);
    auto pan_tool = gen_entity<PanTool>(this);
    auto node_tool = gen_entity<NodeTool>(this);
    auto ribbon_ui = gen_entity<RibbonUI>(this, ribbon_width_in_pixels);
    // ribbon_ui->push_tool(load_tool);
    // ribbon_ui->push_tool(terrain_tool);
    ribbon_ui->push_tool(orbit_tool);
    ribbon_ui->push_tool(pan_tool);
    ribbon_ui->push_tool(node_tool);

    // dinfo = gen_entity<DebugInfo>(1000);
    // dinfo->color->set_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    // dinfo->set_scale(glm::vec3(0.1f));
    // std::string debug_str = Renderer::get_vendor_string() + '\n' +
    //                        Renderer::get_version_string() + '\n' +
    //                        Renderer::get_renderer_string();
    // dinfo->text->set_text(debug_str);
    // dinfo->text->set_center(glm::vec3(0.0f));
  }
  void on_enter() override {}
  void render(float dt) override {
    Renderer::wireframe_mode(false);
    Renderer::enable_depth_testing(false);
    Renderer::enable_blending(true);
    // t += dt;
    // fps_accum += 1.0 / dt;
    // fps_count += 1.0;
    // if (t > 0.5f) {
    //  t = 0.0f;
    //  dinfo->text->set_text(std::to_string(fps_accum / fps_count));
    //  dinfo->text->set_center(glm::vec3(0.0f));
    //  fps_count = 0.0;
    //  fps_accum = 0.0;
    //}
  }
  void on_exit() override {}
  Referenced<DebugInfo> dinfo;
  float t = 0.0f;
  double fps_accum;
  double fps_count;
};

#endif