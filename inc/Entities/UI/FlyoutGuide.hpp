#ifndef FLYOUTGUIDE
#define FLYOUTGUIDE

// MARE
#include "Components/Widget.hpp"
#include "Entities/UI/Button.hpp"
#include "Materials/BasicColorMaterial.hpp"
#include "Meshes/CharMesh.hpp"
#include "Meshes/LineMesh.hpp"

// 3DH
#include "Entities/RibbonTool.hpp"

using namespace mare;

class BackArrowMesh : public SimpleMesh {
public:
  BackArrowMesh() {
    float inner_radius = 0.15f;
    float outer_radius = 0.45f;
    float thickness = outer_radius - inner_radius;
    int sides = 12;
    float theta = math::PI;
    float dtheta = theta / sides;
    std::vector<float> data;
    std::vector<unsigned int> indes;
    set_draw_method(DrawMethod::TRIANGLES);
    // top edge
    float angle = -math::PI / 2.0f;
    for (int i = 0; i < sides + 1; i++) {
      data.push_back(inner_radius * cos(angle));
      data.push_back(inner_radius * sin(angle));
      angle += dtheta;
    }

    angle = -math::PI / 2.0f;
    for (int i = 0; i < sides + 1; i++) {
      data.push_back(outer_radius * cos(angle));
      data.push_back(outer_radius * sin(angle));
      angle += dtheta;
    }

    for (int i = 0; i < sides; i++) {
      indes.push_back(i);
      indes.push_back(sides + i + 1);
      indes.push_back(i + 1);
      indes.push_back(i + 1);
      indes.push_back(sides + i + 1);
      indes.push_back(sides + i + 2);
    }

    // arrow tip
    data.push_back(0.0f);
    data.push_back(outer_radius + 0.5 * thickness);
    data.push_back(-thickness);
    data.push_back((outer_radius + inner_radius) / 2.0f);
    data.push_back(0.0f);
    data.push_back(inner_radius - 0.5 * thickness);
    indes.push_back(2 * sides + 2);
    indes.push_back(2 * sides + 3);
    indes.push_back(2 * sides + 4);

    // Buffer
    Scoped<Buffer<float>> vertex_buffer =
        Renderer::gen_buffer<float>(&data[0], data.size() * sizeof(float));
    vertex_buffer->set_format({{AttributeType::POSITION_2D, "position"}});

    Scoped<Buffer<unsigned int>> index_buffer = Renderer::gen_buffer<uint32_t>(
        &indes[0], indes.size() * sizeof(uint32_t));

    add_geometry_buffer(std::move(vertex_buffer));
    set_index_buffer(std::move(index_buffer));
  }
};

template <typename T> class FlyoutGuideControls;
template <typename T> class FlyoutGuideRenderer;

template <typename T> class FlyoutGuide : public UIElement {
public:
  FlyoutGuide(Layer *layer, std::string title_str)
      : UIElement(layer, util::Rect()) {
    gen_system<FlyoutGuideControls<T>>();
    gen_system<FlyoutGuideRenderer<T>>();
    bounds = util::Rect();
    title = gen_ref<CharMesh>(title_str, 1.0f / 17.0f);
    div_bar = gen_ref<LineMesh>(0.05f);
    back_arrow_mesh = gen_ref<BackArrowMesh>();
    back_button = gen_ref<Button<T>>(layer, bounds, "", back_arrow_mesh);
    title_color = gen_ref<BasicColorMaterial>();
    title_color->set_color({0.0f, 0.0f, 0.0f, 1.0f});
  }
  void rescale() override {
    // set bounds of back_button
    float bounds_height = bounds.top() - bounds.bottom();
    float bounds_width = bounds.right() - bounds.left();
    util::Rect back_button_bounds{};
    back_button_bounds.set_rect(
        {bounds.right() - bounds_height, bounds.bottom()},
        {bounds.right(), bounds.top()});
    back_button->set_bounds(back_button_bounds);
    // move title and div bar to bounds
    title->set_scale({bounds_height, bounds_height, 1.0f});
    title->set_position({bounds.left(), bounds.top(), 0.0f});
    div_bar->set_scale({bounds_width, bounds_height, 1.0f});
    div_bar->set_position(
        {bounds.left() + 0.5 * bounds_width, bounds.bottom(), 0.0f});
  }

  Referenced<CharMesh> title;
  Referenced<LineMesh> div_bar;
  Referenced<BackArrowMesh> back_arrow_mesh;
  Referenced<Button<T>> back_button;
  Referenced<BasicColorMaterial> title_color;
};

template <typename T>
class FlyoutGuideControls : public ControlsSystem<FlyoutGuide<T>> {
public:
  bool on_mouse_button(const RendererInput &input,
                       FlyoutGuide<T> *flyout_guide) override {
    // forward callback
    return flyout_guide->back_button->get_system<ButtonControls<T>>()
        ->on_mouse_button(input, flyout_guide->back_button.get());
  }
};

template <typename T>
class FlyoutGuideRenderer : public RenderSystem<FlyoutGuide<T>> {
public:
  void render(float dt, Camera *camera, FlyoutGuide<T> *flyout_guide) override {
    // forward rendering of back button
    flyout_guide->back_button->set_position(flyout_guide->get_position());
    flyout_guide->back_button->get_system<PacketRenderer>()->render(
        dt, camera, flyout_guide->back_button.get());
    // Render Title
    flyout_guide->title->render(camera, flyout_guide->title_color.get(),
                                flyout_guide);
    // Render div bar
    flyout_guide->div_bar->render(camera, flyout_guide->title_color.get(),
                                  flyout_guide);
  }
};

#endif