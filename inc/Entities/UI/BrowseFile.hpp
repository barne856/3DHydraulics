#ifndef BROWSEFILE
#define BROWSEFILE

// MARE
#include "Components/RenderPack.hpp"
#include "Components/Widget.hpp"
#include "Entities/UI/Button.hpp"
#include "Materials/BasicColorMaterial.hpp"
#include "Meshes/CharMesh.hpp"
#include "Meshes/CircleMesh.hpp"
#include "Meshes/QuadrangleMesh.hpp"
#include "Systems/Controls/ControlsSystemForwarder.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"
#include "Systems/Rendering/RenderSystemForwarder.hpp"

// 3DH
#include "GDAL/gdal_io.hpp"

using namespace mare;

template <typename T> class BrowseFileRenderer;
// declare button template callback type
template <typename T> using on_file_select_callback = void (*)(T *);
template <typename T>
class BrowseFile : public Widget<std::string>, public RenderPack {
public:
  BrowseFile(Layer *layer, util::Rect widget_bounds, std::string label,
             std::string extensions = "")
      : Widget(layer, widget_bounds), file_extensions(extensions),
        file_name("") {
    label_mesh = gen_ref<CharMesh>(label, 1.5f / 17.0f);
    file_name_mesh = gen_ref<CharMesh>("", 1.0f / 17.0f, 0.0f, 13 * 20 + 3);
    file_name_box = gen_ref<QuadrangleMesh>();
    circle_mesh = gen_ref<CircleMesh>(12, 1.0f / 6.0f);
    browse_icon = gen_ref<InstancedMesh>(3);
    browse_icon->set_mesh(circle_mesh);
    browse_icon->push_instance(
        glm::translate(glm::mat4(1.0f), {-0.4f, 0.0f, 0.0f}));
    browse_icon->push_instance(
        glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.0f}));
    browse_icon->push_instance(
        glm::translate(glm::mat4(1.0f), {0.4f, 0.0f, 0.0f}));
    browse_button =
        gen_ref<Button<BrowseFile>>(layer, widget_bounds, "", browse_icon);
    browse_button->set_on_click_callback(open_file, this);
    text_material = gen_ref<BasicColorMaterial>();
    text_material->set_color({0.0f, 0.0f, 0.0f, 1.0f});
    box_material = gen_ref<BasicColorMaterial>();
    box_material->set_color({1.0f, 1.0f, 1.0f, 1.0f});

    push_packet({label_mesh, text_material});
    push_packet({file_name_box, box_material});
    push_packet({file_name_mesh, text_material});

    gen_system<BrowseFileRenderer<T>>();
    gen_system<ControlsSystemForwarder>(browse_button.get());
    gen_system<RenderSystemForwarder>(browse_button.get());
    gen_system<PacketRenderer>();
    rescale();
  }
  void rescale() override {
    // rescale button
    util::Rect button_bounds{};
    button_bounds.set_rect({bounds.right() - bounds.height(), bounds.bottom()},
                           {bounds.right(), bounds.top()});
    browse_button->set_bounds(button_bounds);
    // rescale file name box
    file_name_box->set_scale({0.75f * bounds.width() - 1.25f * bounds.height(),
                              bounds.height(), 1.0f});
    file_name_box->set_position(
        {bounds.left() +
             0.5f * (0.75f * bounds.width() - 1.25f * bounds.height()) +
             0.25f * bounds.width(),
         bounds.bottom() + 0.5f * bounds.height(), 0.0f});
    // rescale file name mesh
    float boarder_thickness = 0.15f * bounds.height();
    max_file_name_chars = static_cast<uint32_t>(
        2.0f * (0.75f * bounds.width() - 1.25f * bounds.height()) /
        (bounds.height() - 2.0f * boarder_thickness));
    set_file_name_mesh_string();
    file_name_mesh->set_scale({bounds.height() - 2.0f * boarder_thickness,
                               bounds.height() - 2.0f * boarder_thickness,
                               1.0f});
    file_name_mesh->set_position(
        {bounds.left() + (1.0f / 4.0f) * bounds.width() + boarder_thickness,
         bounds.top() - boarder_thickness, 0.0f});
    // rescale label mesh
    label_mesh->set_scale({bounds.height(), bounds.height(), 1.0f});
    label_mesh->set_position({bounds.left(), bounds.top(), 0.0f});
  }
  static void open_file(BrowseFile *browse_file) {
    if (browse_file->file_extensions.empty()) {
      browse_file->file_name = gdal_input::open_file_dialog();
    } else {
      browse_file->file_name =
          gdal_input::open_file_dialog(browse_file->file_extensions.c_str());
    }
    browse_file->set_file_name_mesh_string();
    browse_file->on_file_select_func(browse_file->callback_entity_);
  }
  void set_file_name_mesh_string() {
    std::string str = file_name;
    // Remove directory if present.
    const size_t last_slash_idx = str.find_last_of("\\/");
    if (std::string::npos != last_slash_idx) {
      str.erase(0, last_slash_idx + 1);
    }
    // Remove extension if present.
    const size_t period_idx = str.rfind('.');
    if (std::string::npos != period_idx) {
      str.erase(period_idx);
    }
    if (str.length() > max_file_name_chars) {
      str = str.substr(0, max_file_name_chars - 3) + "...";
    }
    file_name_mesh->set_text(str);
  }
  void set_on_file_select_callback(on_file_select_callback<T> callback_func,
                                   T *callback_entity) {
    callback_entity_ = callback_entity;
    on_file_select_func = callback_func;
  }

  std::string file_extensions;
  Referenced<Button<BrowseFile>> browse_button;
  std::string file_name;
  on_file_select_callback<T> on_file_select_func = nullptr;
  T *callback_entity_ = nullptr;

private:
  uint32_t max_file_name_chars = 0;
  Referenced<CharMesh> label_mesh;
  Referenced<CharMesh> file_name_mesh;
  Referenced<QuadrangleMesh> file_name_box;
  Referenced<CircleMesh> circle_mesh;
  Referenced<InstancedMesh> browse_icon;
  Referenced<BasicColorMaterial> text_material;
  Referenced<BasicColorMaterial> box_material;
};

template <typename T>
class BrowseFileRenderer : public RenderSystem<BrowseFile<T>> {
public:
  void render(float dt, Camera *camera, BrowseFile<T> *browse_file) override {
    browse_file->browse_button->set_position(browse_file->get_position());
  }
};

#endif