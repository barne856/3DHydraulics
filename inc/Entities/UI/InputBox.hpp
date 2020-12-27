#ifndef INPUTBOX
#define INPUTBOX

// MARE
#include "Components/RenderPack.hpp"
#include "Components/Widget.hpp"
#include "Entities/UI/Dropdown.hpp"
#include "Entities/UI/TextBox.hpp"
#include "Materials/BasicColorMaterial.hpp"
#include "Systems/Controls/ControlsSystemForwarder.hpp"
#include "Systems/Rendering/RenderSystemForwarder.hpp"

using namespace mare;

template <typename T> class InputBox : public UIElement, public RenderPack {
public:
  InputBox(Layer *layer, std::string label_str, bool is_number = false,
           std::vector<std::string> units = {"ft", "m", "in"})
      : UIElement(layer, util::Rect()) {
    label = gen_ref<CharMesh>(label_str, 1.5f / 17.0f);
    label_material = gen_ref<BasicColorMaterial>();
    label_material->set_color({0.0f, 0.0f, 0.0f, 1.0f});
    input_box =
        gen_ref<TextBox>(layer, bounds, 1, 1.0f / 17.0f, 1.0f / 17.0f, 100);
    push_packet({label, label_material});
    gen_system<RenderSystemForwarder>(input_box.get(), this);
    gen_system<ControlsSystemForwarder>(input_box.get());
    gen_system<PacketRenderer>();
    if (is_number) {
      unit_dropdown = gen_ref<Dropdown<T>>(layer, bounds, "");
      unit_dropdown->set_selection_options(units, 4);
      gen_system<ControlsSystemForwarder>(unit_dropdown.get());
      gen_system<RenderSystemForwarder>(unit_dropdown.get(), this);
    }
  }
  void rescale() override {
    glm::vec2 bounds_center = get_rect_center(bounds);
    // rescale label
    label->set_scale({bounds.height(), bounds.height(), 1.0f});
    label->set_position({bounds.left(), bounds.top(), 0.0f});
    // rescale input box
    if (unit_dropdown) {
      util::Rect field_bounds{};
      field_bounds.set_rect(
          {bounds_center.x - 0.25f * bounds.width(), bounds.bottom()},
          {bounds_center.x + 0.2f * bounds.width(), bounds.top()});
      input_box->set_bounds(field_bounds);
      // rescale unit dropdown
      util::Rect unit_bounds{};
      unit_bounds.set_rect(
          {bounds_center.x + 0.25f * bounds.width(), bounds.bottom()},
          {bounds.right(), bounds.top()});
      unit_dropdown->set_bounds(unit_bounds);
    } else {
      util::Rect field_bounds{};
      field_bounds.set_rect(
          {bounds_center.x - 0.25f * bounds.width(), bounds.bottom()},
          {bounds_center.x + 0.5f * bounds.width(), bounds.top()});
      input_box->set_bounds(field_bounds);
    }
  }
  std::string get_input_value_as_string() { return input_box->get_value(); }
  float get_input_value_as_float() { return std::stof(input_box->get_value()); }
  std::string get_units() { return unit_dropdown->get_value(); }
  Referenced<CharMesh> label;
  Referenced<BasicColorMaterial> label_material;
  Referenced<TextBox> input_box;
  Referenced<Dropdown<T>> unit_dropdown = nullptr;
};

#endif