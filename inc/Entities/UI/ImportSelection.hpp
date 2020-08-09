#ifndef IMPORTSELECTION
#define IMPORTSELECTION

// MARE
#include "Components/RenderPack.hpp"
#include "Components/Widget.hpp"
#include "Entities/UI/Dropdown.hpp"
#include "Materials/BasicColorMaterial.hpp"
#include "Meshes/CharMesh.hpp"
#include "Systems/Controls/ControlsSystemForwarder.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"
#include "Systems/Rendering/RenderSystemForwarder.hpp"

// 3DH
#include "Entities/RibbonTool.hpp"

using namespace mare;

template <typename T> class ImportSelectionRenderer;

template <typename T>
class ImportSelection : public UIElement, public RenderPack {
public:
  ImportSelection(Layer *layer, std::string label_str,
                  bool use_unit_dropdown = false)
      : UIElement(layer, util::Rect()) {
    label = gen_ref<CharMesh>(label_str, 1.5f / 17.0f);
    label_color = gen_ref<BasicColorMaterial>();
    label_color->set_color({0.0f, 0.0f, 0.0f, 1.0f});
    field_dropdown = gen_ref<Dropdown<T>>(layer, bounds, "");
    gen_system<ImportSelectionRenderer<T>>();
    gen_system<ControlsSystemForwarder>(field_dropdown.get());
    gen_system<RenderSystemForwarder>(field_dropdown.get());
    if (use_unit_dropdown) {
      unit_dropdown = gen_ref<Dropdown<T>>(layer, bounds, "");
      gen_system<ControlsSystemForwarder>(unit_dropdown.get());
      gen_system<RenderSystemForwarder>(unit_dropdown.get());
    } else {
      unit_dropdown = nullptr;
    }
    gen_system<PacketRenderer>();
    push_packet({label, label_color});
  }
  void rescale() override {
    glm::vec2 bounds_center = get_rect_center(bounds);
    // rescale label
    label->set_scale({bounds.height(), bounds.height(), 1.0f});
    label->set_position({bounds.left(), bounds.top(), 0.0f});
    // rescale unit dropdown
    if (unit_dropdown) {
      // rescale field dropdown
      util::Rect field_bounds{};
      field_bounds.set_rect(
          {bounds_center.x - 0.25f * bounds.width(), bounds.bottom()},
          {bounds_center.x + 0.25f * bounds.width(), bounds.top()});
      field_dropdown->set_bounds(field_bounds);
      // rescale unit dropdown
      util::Rect unit_bounds{};
      unit_bounds.set_rect(
          {bounds_center.x + 0.25f * bounds.width(), bounds.bottom()},
          {bounds.right(), bounds.top()});
      unit_dropdown->set_bounds(unit_bounds);
    } else {
      // rescale field dropdown
      util::Rect field_bounds{};
      field_bounds.set_rect(
          {bounds_center.x - 0.25f * bounds.width(), bounds.bottom()},
          {bounds_center.x + 0.5f * bounds.width(), bounds.top()});
      field_dropdown->set_bounds(field_bounds);
    }
  }
  void set_field_dropdown_selection_options(std::vector<std::string> options) {
    field_dropdown->set_selection_options(options, 4);
  }
  void set_unit_dropdown_selection_options(std::vector<std::string> options) {
    unit_dropdown->set_selection_options(options, 4);
  }

  Referenced<CharMesh> label;
  Referenced<BasicColorMaterial> label_color;
  Referenced<Dropdown<T>> field_dropdown;
  Referenced<Dropdown<T>> unit_dropdown;
};

template <typename T>
class ImportSelectionRenderer : public RenderSystem<ImportSelection<T>> {
public:
  void render(float dt, Camera *camera,
              ImportSelection<T> *import_selection) override {
    import_selection->field_dropdown->set_position(
        import_selection->get_position());
    if (import_selection->unit_dropdown) {
      import_selection->unit_dropdown->set_position(
          import_selection->get_position());
    }
  }
};

#endif