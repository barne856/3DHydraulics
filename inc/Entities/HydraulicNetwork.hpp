#ifndef HYDRAULICNETWORK
#define HYDRAULICNETWORK

// MARE
#include "Mare.hpp"
#include "Materials/PhongMaterial.hpp"
#include "Meshes/CylinderMesh.hpp"
#include "Systems.hpp"

using namespace mare;

// Standard Library
#include <unordered_map>

// 3DH
class NodeLabelBillboards;

/**
 * @brief A Hydraulic Node in a Hydraulic Network.
 * @details Hydraulic Nodes have some number of Hydraulic Links connected to
 * them and have a constant head at steady state. Hydraulic Links are used to
 * calculate the headloss between Hydraulic Nodes.
 * @see HydraulicLink
 */
class HydraulicNode {
public:
  virtual ~HydraulicNode() = 0;
  virtual float surface_area(float depth) = 0;
  virtual float volume(float depth) = 0;
  double easting{0.0};
  double northing{0.0};
  float invert_elevation{0.0f};
  float node_depth{8.0f};
  std::string ID{""};
};

/**
 * @brief A Cylinder Node is a type of Hydraulic Node in a Hydraulic Network.
 */
class CylinderNode : public HydraulicNode {
public:
  float surface_area(float depth);
  float volume(float depth);
  float inner_diameter{4.0f};
};

class CylinderNodeMeshes {
public:
  CylinderNodeMeshes(uint32_t max_nodes);
  void add_node(CylinderNode *node);
  void render(Camera *camera);

private:
  Referenced<CylinderMesh> base_mesh;
  Referenced<InstancedMesh> main_mesh;
  Referenced<PhongMaterial> material;
};

/**
 * @brief A Hydraulic Network class that represents a newtork of Hydraulic Nodes
 * and Hydraulic Links.
 * @details The Hydraulic Network contains all of the information on the
 * geometry of the hydraulic components in the network and is used to render the
 * network.
 * @see HydraulicNode
 * @see HydraulicLink
 */
class HydraulicNetwork : public Entity {
public:
  HydraulicNetwork();
  void add_node(Referenced<HydraulicNode> node);
  void render(Camera *camera);
  static Referenced<HydraulicNetwork> LoadedNetwork;
  glm::dvec2 offset{0.0, 0.0};

private:
  Referenced<CylinderNodeMeshes> cylinder_node_meshes;
  std::unordered_map<std::string, Referenced<HydraulicNode>>
      nodes_; /**< The HydraulicNodes in the HydraulicNetwork.*/
  Referenced<NodeLabelBillboards> node_labels;
};

class HydraulicNetworkRenderer : public RenderSystem<HydraulicNetwork> {
public:
  void render(float dt, Camera *camera, HydraulicNetwork *network) override;
};

#endif