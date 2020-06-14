#ifndef RIBBONTOOL
#define RIBBONTOOL

// MARE
#include "Components/Widget.hpp"
#include "Entity.hpp"
#include "Meshes.hpp"
using namespace mare;

class RibbonTool : public Entity {
public:
  RibbonTool(Layer *layer) : base_layer(layer) {}
  virtual void on_select() = 0;
  virtual void on_deselect() = 0;
  Referenced<Mesh> icon;
  void rescale(float flyout_width) {
    util::Rect bounds;
    bounds.top() = 1.0f;
    bounds.bottom() = 1.0f;
    int window_height = Renderer::get_info().window_height;
    int i = 0;
    for (auto &elem : flyout_elements) {
      float slot_height = static_cast<float>(element_slots[i][0])/static_cast<float>(window_height);
      float slot_hz_padding = static_cast<float>(element_slots[i][1])/static_cast<float>(window_height);
      float slot_vt_padding = static_cast<float>(element_slots[i][2])/static_cast<float>(window_height);
      float prev_slot_vt_padding = (i>0 ? static_cast<float>(element_slots[i-1][2])/static_cast<float>(window_height) : 0.0f);
      float elem_width = flyout_width - 2.0f*slot_hz_padding;
      bounds.left() = -elem_width / 2.0f;
      bounds.right() = elem_width / 2.0f;
      bounds.top() = bounds.bottom() - slot_vt_padding - prev_slot_vt_padding;
      bounds.bottom() = bounds.top()-slot_height + 2.0f*slot_vt_padding;
      elem->set_bounds(bounds);
      i += 1;
    }
  }
  void push_flyout_element(Referenced<UIElement> elem, glm::ivec3 slots) {
    flyout_elements.push_back(elem);
    element_slots.push_back(slots);
  }
  std::vector<Referenced<UIElement>> flyout_elements{};
  std::vector<glm::ivec3> element_slots; // {slots, hz padding, vt padding} in pixels
  Layer *base_layer;
};

#endif