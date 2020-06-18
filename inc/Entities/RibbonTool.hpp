#ifndef RIBBONTOOL
#define RIBBONTOOL

// MARE
#include "Components/Rigidbody.hpp"
#include "Components/Widget.hpp"
#include "Entity.hpp"
#include "Materials/BasicColorMaterial.hpp"
#include "Meshes.hpp"
#include "Meshes/QuadrangleMesh.hpp"
#include "Systems.hpp"
using namespace mare;

// forward declare systems
class RibbonToolRenderer;
class RibbonToolPhysics;
class RibbonToolControls;

/**
 * @brief An abstract class for a RibbonTool.
 * @details The RibbonTools will be pushed onto a stack in the RibbonUI.
 *
 */
class RibbonTool : public Rigidbody {
public:
  /**
   * @brief Construct a new RibbonTool
   *
   * @param layer The Layer that the RibbonUI uses.
   */
  RibbonTool(Layer *layer) : base_layer(layer) {
    flyout = gen_ref<QuadrangleMesh>();
    flyout_material = gen_ref<BasicColorMaterial>();
    flyout_material->set_color(flyout_color);
    gen_system<RibbonToolRenderer>();
    gen_system<RibbonToolPhysics>();
    gen_system<RibbonToolControls>();
  }
  virtual void on_select() = 0;
  virtual void on_deselect() = 0;
  Referenced<Mesh> icon;
  void rescale(float ribbon_width) {
    float scale = base_layer->get_ortho_scale();
    float aspect = Renderer::get_info().window_aspect;
    flyout_closed_position = -scale * aspect - ribbon_width * 1.5f;
    flyout_opened_position = -scale * aspect + ribbon_width * 3.5f;
    flyout_position = glm::mix(flyout_closed_position, flyout_opened_position,
                               flyout_percent_open);
    flyout->set_scale({5.0f * ribbon_width, 2.0f * scale, 1.0f});
    flyout->set_position({flyout_position, 0.0f, 0.0f});

    util::Rect bounds;
    bounds.top() = 1.0f;
    bounds.bottom() = 1.0f;
    int window_height = Renderer::get_info().window_height;
    int i = 0;
    for (auto &elem : flyout_elements) {
      float slot_height = static_cast<float>(element_slots[i][0]) /
                          static_cast<float>(window_height);
      float slot_hz_padding = static_cast<float>(element_slots[i][1]) /
                              static_cast<float>(window_height);
      float slot_vt_padding = static_cast<float>(element_slots[i][2]) /
                              static_cast<float>(window_height);
      float prev_slot_vt_padding =
          (i > 0 ? static_cast<float>(element_slots[i - 1][2]) /
                       static_cast<float>(window_height)
                 : 0.0f);
      float elem_width =
          5.0f * ribbon_width -
          2.0f * slot_hz_padding; // flyout width is 5 times ribbon_width
      bounds.left() = -elem_width / 2.0f;
      bounds.right() = elem_width / 2.0f;
      bounds.top() = bounds.bottom() - slot_vt_padding - prev_slot_vt_padding;
      bounds.bottom() = bounds.top() - slot_height + 2.0f * slot_vt_padding;
      elem->set_bounds(bounds);
      i += 1;
    }
  }
  /**
   * @brief Push a UIElement onto the flyout element stack.
   * @details If there are no UIElements pushed, then the flyout will never
   * open.
   * @param elem The UIElement to push.
   * @param slots The dimensions of the slot in pixels {total slot height with
   * padding, horizontal padding, vertical padding}.
   */
  void push_flyout_element(Referenced<UIElement> elem, glm::ivec3 slots) {
    flyout_elements.push_back(elem);
    element_slots.push_back(slots);
  }
  std::vector<Referenced<UIElement>> flyout_elements{};
  std::vector<glm::ivec3>
      element_slots; // {slots, hz padding, vt padding} in pixels
  Layer *base_layer;
  Referenced<QuadrangleMesh> flyout;
  Referenced<BasicColorMaterial> flyout_material;
  float flyout_closed_position = 0.0f;
  float flyout_opened_position = 0.0f;
  float flyout_position = 0.0f;
  float flyout_percent_open = 0.0f;
  bool flyout_open = false;
  glm::vec4 flyout_color{0.9f, 0.9f, 1.0f, 1.0f};
  bool selected = false;
};

class RibbonToolRenderer : public RenderSystem<RibbonTool> {
public:
  void render(float dt, Camera *camera, RibbonTool *tool) override {
    if (tool->flyout_closed_position != tool->flyout_position) {
      tool->flyout->render(camera, tool->flyout_material.get());
      // render flyout elements
      for (auto &elem : tool->flyout_elements) {
        auto render_systems = elem->get_systems<IRenderSystem>();
        for (auto &rs : render_systems) {
          // render the flyout element using the tool's transform
          elem->set_position({tool->flyout_position, 0.0f, 0.0f});
          rs->render(dt, camera, elem.get());
        }
      }
    }
  }
};

class RibbonToolPhysics : public PhysicsSystem<RibbonTool> {
public:
  void update(float dt, RibbonTool *tool) override {
    // update flyout elements
    for (auto &elem : tool->flyout_elements) {
      auto physics_systems = elem->get_systems<IPhysicsSystem>();
      for (auto &ps : physics_systems) {
        ps->update(dt, elem.get());
      }
    }

    if (tool->flyout_position == tool->flyout_closed_position &&
        tool->flyout_open == false) {
      return;
    }
    if (tool->flyout_position == tool->flyout_opened_position &&
        tool->flyout_open == true) {
      return;
    }
    glm::vec3 force{};
    if (tool->flyout_open) {
      force = k * (tool->flyout_opened_position - tool->flyout_position) -
              c * tool->linear_velocity;
    } else {
      force = k * (tool->flyout_closed_position - tool->flyout_position) -
              c * tool->linear_velocity;
    }
    tool->linear_velocity += force * dt;
    float new_position =
        glm::clamp(tool->flyout_position + tool->linear_velocity.x * dt,
                   tool->flyout_closed_position, tool->flyout_opened_position);
    if (new_position - tool->flyout_closed_position <= tol) {
      new_position = tool->flyout_closed_position;
    }
    if (tool->flyout_opened_position - new_position <= tol) {
      new_position = tool->flyout_opened_position;
    }
    tool->flyout_position = new_position;
    tool->flyout_percent_open =
        (tool->flyout_position - tool->flyout_closed_position) /
        (tool->flyout_opened_position - tool->flyout_closed_position);
    tool->flyout->set_position({tool->flyout_position, 0.0f, 0.0f});
  }
  // Critically damped harmonic oscillator, mass = 1.
  const float k = 1000.0f; // strength of the spring.
  const float c =
      2.0f * sqrtf(k);      // strength of the damping (critically damped).
  const float tol = 0.001f; // distance when knob snaps to the lerp_to position.
};

class RibbonToolControls : public ControlsSystem<RibbonTool> {
public:
  bool on_key(const RendererInput &input, RibbonTool *tool) override {
    // loop through flyout elements
    for (auto &elem : tool->flyout_elements) {
      auto control_systems = elem->get_systems<IControlsSystem>();
      for (auto &cs : control_systems) {
        // callback the flyout element
        if(cs->on_key(input, elem.get()))
        {
          return true;
        }
      }
    }
    return false;
  }
  bool on_mouse_button(const RendererInput &input, RibbonTool *tool) override {
    // loop through flyout elements
    for (auto &elem : tool->flyout_elements) {
      auto control_systems = elem->get_systems<IControlsSystem>();
      for (auto &cs : control_systems) {
        // callback the flyout element
        if(cs->on_mouse_button(input, elem.get()))
        {
          return true;
        }
      }
    }
    return false;
  }
  bool on_mouse_move(const RendererInput &input, RibbonTool *tool) override {
    // loop through flyout elements
    for (auto &elem : tool->flyout_elements) {
      auto control_systems = elem->get_systems<IControlsSystem>();
      for (auto &cs : control_systems) {
        // callback the flyout element
        if(cs->on_mouse_move(input, elem.get()))
        {
          return true;
        }
      }
    }
    return false;
  }
  bool on_mouse_wheel(const RendererInput &input, RibbonTool *tool) override {
    // loop through flyout elements
    for (auto &elem : tool->flyout_elements) {
      auto control_systems = elem->get_systems<IControlsSystem>();
      for (auto &cs : control_systems) {
        // callback the flyout element
        if(cs->on_mouse_wheel(input, elem.get()))
        {
          return true;
        }
      }
    }
    return false;
  }
  bool on_resize(const RendererInput &input, RibbonTool *tool) override {
    // loop through flyout elements
    for (auto &elem : tool->flyout_elements) {
      auto control_systems = elem->get_systems<IControlsSystem>();
      for (auto &cs : control_systems) {
        // callback the flyout element
        if(cs->on_resize(input, elem.get()))
        {
          return true;
        }
      }
    }
    return false;
  }
  bool on_char(char character, RibbonTool *tool) override {
    // loop through flyout elements
    for (auto &elem : tool->flyout_elements) {
      auto control_systems = elem->get_systems<IControlsSystem>();
      for (auto &cs : control_systems) {
        // callback the flyout element
        if(cs->on_char(character, elem.get()))
        {
          return true;
        }
      }
    }
    return false;
  }
};

#endif