#ifndef TERRAINTOOL
#define TERRAINTOOL

#include "Entities/RibbonTool.hpp"
#include "Entities/Terrain.hpp"
#include "Entities/UI/Slider.hpp"
#include "Mare.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Systems.hpp"

class TerrainToolRenderer;

class TerrainTool : public RibbonTool {
public:
  TerrainTool(Layer *layer, uint32_t slot_height_in_pixels)
      : RibbonTool(layer) {
    icon = gen_ref<CompositeMesh>();
    auto vertical = gen_ref<QuadrangleMesh>();
    auto horizontal = gen_ref<QuadrangleMesh>();
    vertical->set_scale({0.2f, 0.6f, 1.0f});
    horizontal->set_scale({0.6f, 0.2f, 1.0f});
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(vertical);
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(horizontal);
    util::Rect bounds = util::Rect();
    if (layer) {
      slot_height =
          slot_height_in_pixels; // total slot height including padding

      // vertical exaggeration slider
      ve_slider = gen_ref<Slider>(layer, bounds);
      // alpha slider
      alpha_slider = gen_ref<Slider>(layer, bounds);

      // push data type dropdown
      push_flyout_element(ve_slider,
                          {slot_height, slot_height / 5, slot_height / 10});
      push_flyout_element(alpha_slider,
                          {slot_height, slot_height / 5, slot_height / 10});
      gen_system<TerrainToolRenderer>();
    }
  }
  void on_select() override {}
  void on_deselect() override {}
  Referenced<Slider> ve_slider;
  Referenced<Slider> alpha_slider;

private:
  uint32_t slot_height;
};

class TerrainToolRenderer : public RenderSystem<TerrainTool> {
public:
  void render(float dt, Camera *camera, TerrainTool *tool) override {
    auto scene = Renderer::get_info().scene;
    if (Terrain *terra = scene->get_entity<Terrain>()) {
      terra->set_vert_exag(tool->ve_slider->get_value() * 10.0f);
      terra->set_alpha(tool->alpha_slider->get_value());
    }
  }
};

#endif