#ifndef LOADTOOL
#define LOADTOOL

#include "Entities/RibbonTool.hpp"
#include "Entities/UI/Button.hpp"
#include "Entities/UI/ColorPicker.hpp"
#include "Entities/UI/Slider.hpp"
#include "Entities/UI/Switch.hpp"
#include "Entities/UI/TextBox.hpp"
#include "Meshes/QuadrangleMesh.hpp"
#include "Renderer.hpp"
using namespace mare;

class LoadTool : public RibbonTool {
public:
  LoadTool(Layer *layer) : RibbonTool(layer) {
    icon = gen_ref<CompositeMesh>();
    auto vertical = gen_ref<QuadrangleMesh>();
    auto horizontal = gen_ref<QuadrangleMesh>();
    vertical->set_scale({0.2f, 0.6f, 1.0f});
    horizontal->set_scale({0.6f, 0.2f, 1.0f});
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(vertical);
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(horizontal);
    util::Rect bounds = util::Rect();
    if (layer) {
      int slot_height = 100.0f; // total slot height including padding
      push_flyout_element(gen_ref<Slider>(layer, bounds),
                          {slot_height, slot_height / 3, slot_height / 3});
      push_flyout_element(
          gen_ref<TextBox>(layer, bounds, 1, 0.025f, 0.025f, 280),
          {slot_height, slot_height / 5.0f, slot_height / 10.0f});
      push_flyout_element(gen_ref<Switch>(layer, bounds),
                          {slot_height, 1.75f * slot_height, slot_height / 10});
      push_flyout_element(gen_ref<Button>(layer, bounds, "Button"),
                          {slot_height, slot_height / 5, slot_height / 10});
      push_flyout_element(gen_ref<ColorPicker>(layer, bounds),
                          {5 * slot_height, slot_height / 5, slot_height / 10});
    }
  }
  void on_select() override {}
  void on_deselect() override {}
};

#endif