#ifndef NAVIGATIONTOOLS
#define NAVIGATIONTOOLS

// MARE
#include "Meshes/ArrowMesh2D.hpp"
#include "Meshes/CircleMesh.hpp"
#include "Renderer.hpp"
#include "Systems/Controls/OrbitControls.hpp"

using namespace mare;

// 3DH
#include "Entities/RibbonTool.hpp"

// EXT
#include "gtc/matrix_transform.hpp"

class CycleArrowMesh : public SimpleMesh {
public:
  CycleArrowMesh(float inner_radius, float outer_radius, int sides,
                 float begin_angle, float end_angle,
                 float arrow_head_thickness) {
    float thickness = outer_radius - inner_radius;
    float theta = end_angle - begin_angle;
    float dtheta = theta / sides;
    std::vector<float> data;
    std::vector<unsigned int> indes;
    set_draw_method(DrawMethod::TRIANGLES);
    // top edge
    float angle = begin_angle;
    for (int i = 0; i < sides + 1; i++) {
      data.push_back(inner_radius * cos(angle));
      data.push_back(inner_radius * sin(angle));
      angle += dtheta;
    }

    angle = begin_angle;
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
    float middle = inner_radius + 0.5f * thickness;
    glm::vec2 v1 = {middle + 0.5f * arrow_head_thickness, 0.0f};
    glm::vec2 v2 = {middle, arrow_head_thickness};
    glm::vec2 v3 = {middle - 0.5f * arrow_head_thickness, 0.0f};
    float x_temp = v1[0] * cosf(end_angle) - v1[1] * sinf(end_angle);
    float y_temp = v1[0] * sinf(end_angle) + v1[1] * cosf(end_angle);
    v1[0] = x_temp;
    v1[1] = y_temp;
    x_temp = v2[0] * cosf(end_angle) - v2[1] * sinf(end_angle);
    y_temp = v2[0] * sinf(end_angle) + v2[1] * cosf(end_angle);
    v2[0] = x_temp;
    v2[1] = y_temp;
    x_temp = v3[0] * cosf(end_angle) - v3[1] * sinf(end_angle);
    y_temp = v3[0] * sinf(end_angle) + v3[1] * cosf(end_angle);
    v3[0] = x_temp;
    v3[1] = y_temp;
    data.push_back(v1[0]);
    data.push_back(v1[1]);
    data.push_back(v2[0]);
    data.push_back(v2[1]);
    data.push_back(v3[0]);
    data.push_back(v3[1]);
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

class OrbitTool : public RibbonTool {
public:
  OrbitTool(Layer *layer) : RibbonTool(layer) {
    // The Tool's Icon
    icon = gen_ref<CompositeMesh>();
    auto point_mesh = gen_ref<CircleMesh>(12, 0.05f);
    float inner_radius = 0.25f;
    float outer_radius = 0.3f;
    int sides = 12;
    float gap_angle = 0.75f;
    float begin_angle_1 = (-math::PI + gap_angle) / 2.0f;
    float end_angle_1 = (math::PI - gap_angle) / 2.0f;
    float begin_angle_2 = (math::PI + gap_angle) / 2.0f;
    float end_angle_2 = (3.0f * math::PI - gap_angle) / 2.0f;
    float arrow_head_thickness = 0.125f;
    auto arrow_1 = gen_ref<CycleArrowMesh>(inner_radius, outer_radius, sides,
                                           begin_angle_1, end_angle_1,
                                           arrow_head_thickness);
    auto arrow_2 = gen_ref<CycleArrowMesh>(inner_radius, outer_radius, sides,
                                           begin_angle_2, end_angle_2,
                                           arrow_head_thickness);
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(point_mesh);
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(arrow_1);
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(arrow_2);
  }
  void on_select() override {
    if (auto sys = (Renderer::get_info().scene)->get_system<OrbitControls>()) {
      sys->pan_mode = false;
      sys->left_click_disabled = false;
    }
  }
  void on_deselect() override {}
};

class PanTool : public RibbonTool {
public:
  PanTool(Layer *layer) : RibbonTool(layer) {
    // The Tool's Icon
    icon = gen_ref<InstancedMesh>(4);
    float body_length = 0.2f;
    float body_width = 0.1f;
    float head_length = 0.125f;
    float head_width = 0.2f;
    auto arrow =
        gen_ref<ArrowMesh2D>(body_length, body_width, head_length, head_width);
    std::dynamic_pointer_cast<InstancedMesh>(icon)->set_mesh(arrow);
    std::dynamic_pointer_cast<InstancedMesh>(icon)->push_instance(
        glm::mat4(1.0f));
    std::dynamic_pointer_cast<InstancedMesh>(icon)->push_instance(
        glm::rotate(glm::mat4(1.0f), -math::PI/2.0f, {0.0f, 0.0f, 1.0f}));
    std::dynamic_pointer_cast<InstancedMesh>(icon)->push_instance(
        glm::rotate(glm::mat4(1.0f), math::PI/2.0f, {0.0f, 0.0f, 1.0f}));
    std::dynamic_pointer_cast<InstancedMesh>(icon)->push_instance(
        glm::rotate(glm::mat4(1.0f), math::PI, {0.0f, 0.0f, 1.0f}));
  }
  void on_select() override {
    if (auto sys = (Renderer::get_info().scene)->get_system<OrbitControls>()) {
      sys->pan_mode = true;
      sys->left_click_disabled = false;
    }
  }
  void on_deselect() override {}
};

#endif