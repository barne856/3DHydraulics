#ifndef RIBBONUI
#define RIBBONUI

// MARE
#include "Components/RenderPack.hpp"
#include "Components/Widget.hpp"
#include "Materials/BasicColorMaterial.hpp"
#include "Materials/VertexColorMaterial.hpp"
#include "Meshes/CircleMesh.hpp"
#include "Meshes/QuadrangleMesh.hpp"
#include "Systems.hpp"
using namespace mare;

// TOOLS
#include "Entities/RibbonTool.hpp"

// Standard Library
#include <vector>

// External Libraries
#include "glm.hpp"

// forward declare system
class RibbonUIControls;
class RibbonUIRenderer;

/**
 * @brief A Ribbon UIElement
 * @details The RibbonUI is the main way of interacting with the program. The
 * User will select tools, change settings, and perform tasks using the
 * RibbonUI. The layer which this element is attached to must be centered on
 * (0,0,0) with an orthographic projection with the y-axis running vertically
 * and x-axis runnign horizontally.
 *
 */
class RibbonUI : public UIElement {
public:
  /**
   * @brief Construct a new RibbonUI.
   *
   * @param layer The base layer that the RibbonUI is attached to.
   * @param ribbon_width_in_pixels The width of the ribbon in pixels, used to
   * size the flyout and widgets as well. Default is 100.
   */
  RibbonUI(Layer *layer, uint32_t ribbon_width_in_pixels = 100)
      : UIElement(layer, util::Rect()), ribbon_width(ribbon_width_in_pixels) {
    // Generate the Render System so that the render packets will be
    // rendered.
    gen_system<RibbonUIRenderer>();
    // Generate Controls System
    gen_system<RibbonUIControls>();

    // Generate the assets used for the RibbonUI.
    ribbon = gen_ref<QuadrangleMesh>();
    selection = gen_ref<QuadrangleMesh>();
    shadow = gen_ref<QuadrangleMesh>(shadow_color_dark, shadow_color_light,
                                     shadow_color_light, shadow_color_dark);
    ribbon_material = gen_ref<BasicColorMaterial>();
    ribbon_material->set_color(ribbon_color);
    tool_material = gen_ref<BasicColorMaterial>();
    tool_material->set_color(tool_color);
    selection_material = gen_ref<BasicColorMaterial>();
    selection_material->set_color(selection_color);
    shadow_material = gen_ref<VertexColorMaterial>();

    // Set the initial bounds to be on the left side of the screen and rescale.
    rescale();
  }
  /**
   * @brief Rescales the RibbonUI to fit the screen correctly.
   *
   */
  void rescale() override {
    float scale = get_layer()->get_ortho_scale();
    float aspect = Renderer::get_info().window_aspect;
    float ribbon_width_layer =
        scale * aspect * static_cast<float>(ribbon_width) /
        static_cast<float>(Renderer::get_info().window_width);
    bounds.left() = -scale * aspect;
    bounds.right() = -scale * aspect + ribbon_width_layer;
    bounds.top() = scale;
    bounds.bottom() = -scale;
    shadow->set_scale({ribbon_width_layer / 10.0f, 2.0f * scale, 1.0f});
    shadow->set_position(
        {-scale * aspect + ribbon_width_layer * 21.0f / 20.0f, 0.0f, 0.0f});
    ribbon->set_scale({ribbon_width_layer, 2.0f * scale, 1.0f});
    ribbon->set_position(
        {-scale * aspect + ribbon_width_layer / 2.0f, 0.0f, 0.0f});
    // rescale tools (single column for now)
    selection->set_scale(glm::vec3(ribbon_width_layer));
    selection->set_position(
        {bounds.left() + 0.5f * ribbon_width_layer,
         bounds.top() -
             (static_cast<float>(tool_index) + 0.5f) * ribbon_width_layer,
         0.0f});
    for (int i = 0; i < tools.size(); i++) {
      tools[i]->icon->set_scale(glm::vec3(ribbon_width_layer));
      tools[i]->icon->set_position(
          {bounds.left() + 0.5f * ribbon_width_layer,
           bounds.top() - (static_cast<float>(i) + 0.5f) * ribbon_width_layer,
           0.0f});
      tools[i]->rescale(ribbon_width_layer);
    }
  }
  void push_tool(Referenced<RibbonTool> tool) {
    tools.push_back(tool);
    rescale();
  }
  /**
   * @brief Runs when a new tool is selected and rescales the UI.
   *
   * @param new_tool_index The index of the new tool.
   */
  void select_tool(uint32_t new_tool_index) {
    if (tool_index >= 0) {
      tools[tool_index]->on_deselect();
      tools[tool_index]->flyout_open = false;
      tools[tool_index]->selected = false;
    }
    tool_index = new_tool_index;
    tools[new_tool_index]->on_select();
    tools[new_tool_index]->selected = true;
    if (tools[new_tool_index]->flyout_elements.size() == 0) {
      tools[new_tool_index]->flyout_open = false;
    } else {
      tools[new_tool_index]->flyout_open = true;
    }

    rescale();
  }
  /**
   * @brief The number of tools in the RibbonUI.
   *
   * @return The number of tools in the RibbonUI.
   */
  uint32_t tool_count() { return tools.size(); }
  void on_focus() override {}
  void on_unfocus() override {}

  std::vector<Referenced<RibbonTool>> tools;
  Referenced<QuadrangleMesh> ribbon;
  Referenced<QuadrangleMesh> selection;
  Referenced<QuadrangleMesh> shadow;
  Referenced<BasicColorMaterial> ribbon_material;
  Referenced<BasicColorMaterial> tool_material;
  Referenced<BasicColorMaterial> selection_material;
  Referenced<VertexColorMaterial> shadow_material;
  glm::vec4 ribbon_color{0.9f, 0.9f, 1.0f, 1.0f};
  glm::vec4 tool_color{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 selection_color{0.17f, 0.45f, 1.0f, 1.0f};
  glm::vec4 shadow_color_dark{0.0f, 0.0f, 0.0f, 0.3f};
  glm::vec4 shadow_color_light{0.0f, 0.0f, 0.0f, 0.0f};
  uint32_t ribbon_width; // in pixels
  int tool_index = -1;
};

class RibbonUIControls : public ControlsSystem<RibbonUI> {
public:
  bool on_mouse_button(const RendererInput &input, RibbonUI *ribbon) override {
    glm::vec2 layer_coords = ribbon->get_model_coords();
    if (input.LEFT_MOUSE_JUST_PRESSED && ribbon->is_cursor_in_bounds()) {
      float width = ribbon->get_right() - ribbon->get_left();
      float top = ribbon->get_top();
      uint32_t i = 1;
      while (top - static_cast<float>(i) * width > layer_coords.y) {
        i++;
      }
      i--;
      if (i < ribbon->tool_count() && i != ribbon->tool_index) {
        ribbon->select_tool(i);
      }
      if (ribbon->tool_index >= 0 &&
          ribbon->tools[ribbon->tool_index]->flyout_elements.size()) {
        ribbon->tools[ribbon->tool_index]->flyout_open = true;
      }
      return true;
    }
    float scale = ribbon->get_layer()->get_ortho_scale();
    float aspect = Renderer::get_info().window_aspect;
    float ribbon_width_layer =
        scale * aspect * static_cast<float>(ribbon->ribbon_width) /
        static_cast<float>(Renderer::get_info().window_width);
    if (ribbon->tool_index >= 0 && input.LEFT_MOUSE_JUST_PRESSED &&
        layer_coords.x > -scale * aspect + 6.0f * ribbon_width_layer) {
      ribbon->tools[ribbon->tool_index]->flyout_open = false;
    }
    return false;
  }
};

class RibbonUIRenderer : public RenderSystem<RibbonUI> {
public:
  void render(float dt, Camera *camera, RibbonUI *ribbon) override {
    ribbon->shadow->render(camera, ribbon->shadow_material.get());
    ribbon->ribbon->render(camera, ribbon->ribbon_material.get());
    ribbon->selection->render(camera, ribbon->selection_material.get());
    for (int i = 0; i < ribbon->tools.size(); i++) {
      if (i == ribbon->tool_index) {
        ribbon->tools[i]->icon->render(camera, ribbon->ribbon_material.get());
      } else {
        ribbon->tools[i]->icon->render(camera, ribbon->tool_material.get());
      }
    }
  }
};

#endif