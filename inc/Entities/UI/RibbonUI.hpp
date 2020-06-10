#ifndef RIBBONUI
#define RIBBONUI

// MARE
#include "Materials/BasicColorMaterial.hpp"
#include "Meshes/CircleMesh.hpp"
#include "Meshes/QuadrangleMesh.hpp"
#include "Components/RenderPack.hpp"
#include "Components/Widget.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"
using namespace mare;

// External Libraries
#include "glm.hpp"

/**
 * @brief A Ribbon UIElement
 * @details The RibbonUI is the main way of interacting with the program. The
 * User will select tools, change settings, and perform tasks using the
 * RibbonUI. The layer which this element is attached to must be centered on
 * (0,0,0) with an orthographic projection with the y-axis running vertically
 * and x-axis runnign horizontally.
 *
 */
class RibbonUI : public Entity, public UIElement, public RenderPack {
public:
  /**
   * @brief Construct a new RibbonUI.
   *
   * @param layer The base layer that the RibbonUI is attached to.
   * @param bounds The bounds of the RibbonUI used to scale to element and test
   * for input.
   */
  RibbonUI(Layer *layer) : UIElement(layer, util::Rect()) {
    // Generate the Packet Renderer System so that the render packets will be
    // rendered.
    gen_system<PacketRenderer>();

    // Generate the assets used for the RibbonUI.
    ribbon = gen_ref<QuadrangleMesh>();
    resize_bar = gen_ref<QuadrangleMesh>();
    circle = gen_ref<CircleMesh>(6, 0.5f);
    resize_bar_decoration = gen_ref<InstancedMesh>(3);
    resize_bar_decoration->set_mesh(circle);
    resize_bar_decoration->push_instance(glm::mat4(1.0f));
    resize_bar_decoration->push_instance(glm::mat4(1.0f));
    resize_bar_decoration->push_instance(glm::mat4(1.0f));
    ribbon_material = gen_ref<BasicColorMaterial>();
    ribbon_material->set_color(ribbon_color);
    accent_material = gen_ref<BasicColorMaterial>();
    accent_material->set_color(accent_color);

    // Push the render packets to the rendering system in the order that they
    // will be rendered.
    push_packet({ribbon, ribbon_material});
    push_packet({resize_bar, accent_material});
    push_packet({resize_bar_decoration, ribbon_material});

    // Set the initial bounds to be on the left side of the screen and rescale.
    float scale = get_layer()->get_ortho_scale();
    float aspect = Renderer::get_info().window_aspect;
    bounds.left() = -scale * aspect;
    bounds.right() = -scale * aspect + ribbon_width;
    bounds.top() = scale;
    bounds.bottom() = -scale;
    rescale();
  }
  /**
   * @brief Rescales the RibbonUI to fit the screen correctly.
   * 
   */
  void rescale() override {
    float scale = get_layer()->get_ortho_scale();
    float aspect = Renderer::get_info().window_aspect;
    ribbon->set_scale({ribbon_width, scale, 1.0f});
    ribbon->set_position({-scale * aspect + ribbon_width / 2.0f, 0.0f, 0.0f});
    resize_bar->set_scale({resize_bar_width, scale, 1.0f});
    resize_bar->set_position(
        {-scale * aspect + ribbon_width - resize_bar_width / 2.0f, 0.0f, 0.0f});
    (*resize_bar_decoration)[0] =
        glm::translate(glm::mat4(1.0f), {-scale * aspect + ribbon_width -
                                             resize_bar_width / 2.0f,
                                         2.0f * resize_bar_width, 0.0f}) *
        glm::scale(glm::mat4(1.0f), glm::vec3(resize_bar_width / 2.0f));
    (*resize_bar_decoration)[1] =
        glm::translate(glm::mat4(1.0f), {-scale * aspect + ribbon_width -
                                             resize_bar_width / 2.0f,
                                         0.0f, 0.0f}) *
        glm::scale(glm::mat4(1.0f), glm::vec3(resize_bar_width / 2.0f));
    (*resize_bar_decoration)[2] =
        glm::translate(glm::mat4(1.0f), {-scale * aspect + ribbon_width -
                                             resize_bar_width / 2.0f,
                                         -2.0f * resize_bar_width, 0.0f}) *
        glm::scale(glm::mat4(1.0f), glm::vec3(resize_bar_width / 2.0f));
  }
  void on_focus() override {}
  void on_unfocus() override {}

private:
  Referenced<QuadrangleMesh> ribbon;
  Referenced<QuadrangleMesh> resize_bar;
  Referenced<CircleMesh> circle;
  Referenced<InstancedMesh> resize_bar_decoration;
  Referenced<BasicColorMaterial> ribbon_material;
  Referenced<BasicColorMaterial> accent_material;
  glm::vec4 ribbon_color{0.9f, 0.9f, 0.9f, 1.0f};
  glm::vec4 accent_color{0.1f, 0.1f, 0.1f, 1.0f};
  float ribbon_width = 0.1f;
  float resize_bar_width = 0.02f;
};

#endif