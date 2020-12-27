#ifndef NODELABELBILLBOARDS
#define NODELABELBILLBOARDS

// MARE
#include "Components/RenderPack.hpp"
#include "Materials/PhongMaterial.hpp"
#include "Meshes/CharMesh.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"

using namespace mare;

// 3DH
#include "Entities/HydraulicNetwork.hpp"

// Standard Library
#include <vector>

class NodeLabelBillboardsRenderer;

class NodeLabelBillboards : public RenderPack {
public:
  NodeLabelBillboards() {
    material = gen_ref<PhongMaterial>();
    material->set_ambient_color(glm::vec4(0.8f, 0.8f, 0.2f, 1.0f));
    auto light = gen_ref<Spotlight>();
    light->set_position({10.0f, 10.0f, 10.0f});
    material->set_light(light);
    gen_system<NodeLabelBillboardsRenderer>();
    gen_system<PacketRenderer>();
  }
  void add_label(HydraulicNode *node) {
    auto label = gen_ref<CharMesh>(node->ID, 1.0f / 17.0f, 2.0f / 17.0f);
    auto network = HydraulicNetwork::LoadedNetwork.get();
    glm::vec3 label_center = {
        node->easting - network->offset.x, node->northing - network->offset.y,
        node->invert_elevation + node->node_depth + 10.0f};
    label->set_scale(glm::vec3(10.0f));
    label->set_center(label_center);
    auto temp_pos = label->get_position();
    temp_pos.z = label_center.z;
    label->set_position(temp_pos);
    labels.push_back(label);
    push_packet({label, material});
  }

  std::vector<Referenced<CharMesh>> labels;

private:
  Referenced<PhongMaterial> material;
};

class NodeLabelBillboardsRenderer : public RenderSystem<NodeLabelBillboards> {
public:
  void render(float dt, Camera *camera, NodeLabelBillboards *labels) override {
    for (auto &label : labels->labels) {
      label->set_rotation_matrix(camera->get_rotation_matrix());
    }
  }
};

#endif