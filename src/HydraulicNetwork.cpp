#include "Entities/HydraulicNetwork.hpp"
#include "Scenes/MainScene.hpp"
#include "Systems/Rendering/RenderSystemForwarder.hpp"
#include "Entities/NodeLabelBillboards.hpp"

Referenced<HydraulicNetwork> HydraulicNetwork::LoadedNetwork = nullptr;

// Hydraulic Nodes
HydraulicNode::~HydraulicNode() {}

// Clyinder Nodes
CylinderNodeMeshes::CylinderNodeMeshes(uint32_t max_nodes) {
  base_mesh = gen_ref<CylinderMesh>(0.0f, 2.0f * math::PI, 24);
  main_mesh = gen_ref<InstancedMesh>(max_nodes);
  main_mesh->set_mesh(base_mesh);
  material = gen_ref<PhongMaterial>();
  material->set_ambient_color(glm::vec4(0.6f, 0.6f, 0.6f, 1.0f));
  auto light = gen_ref<Spotlight>();
  light->set_position({10.0f, 10.0f, 10.0f});
  material->set_light(light);
}

void CylinderNodeMeshes::add_node(CylinderNode *node) {
  HydraulicNetwork *network = HydraulicNetwork::LoadedNetwork.get();
  glm::vec3 position = {node->easting - network->offset.x,
                        node->northing - network->offset.y,
                        node->invert_elevation};
  glm::mat4 scale =
      glm::scale(glm::mat4(1.0f), {node->inner_diameter, node->inner_diameter,
                                   node->node_depth});
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), position);
  main_mesh->push_instance(trans * scale);
}

void CylinderNodeMeshes::render(Camera *camera) {
  main_mesh->render(camera, material.get(), main_mesh.get());
}

float CylinderNode::surface_area(float depth) {
  return math::PI * inner_diameter * inner_diameter / 4.0f;
}

float CylinderNode::volume(float depth) { return surface_area(depth) * depth; }

// Hydraulic Network
HydraulicNetwork::HydraulicNetwork() {
  cylinder_node_meshes = gen_ref<CylinderNodeMeshes>(1000);
  node_labels = gen_ref<NodeLabelBillboards>();
  gen_system<RenderSystemForwarder>(node_labels.get());
  gen_system<HydraulicNetworkRenderer>();
}

void HydraulicNetwork::add_node(Referenced<HydraulicNode> node) {
  nodes_.insert_or_assign(node->ID, node);
  if (auto n = std::dynamic_pointer_cast<CylinderNode>(node)) {
    cylinder_node_meshes->add_node(n.get());
  }
  node_labels->add_label(node.get());
}

void HydraulicNetwork::render(Camera *camera) {
  cylinder_node_meshes->render(camera);
}

void HydraulicNetworkRenderer::render(float dt, Camera *camera,
                                      HydraulicNetwork *network) {
  network->render(camera);
}