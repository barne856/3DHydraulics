#ifndef HYDRAULICNETWORK
#define HYDRAULICNETWORK

// MARE
#include "Components/RenderPack.hpp"
#include "Entities/Spotlight.hpp"
#include "Mare.hpp"
#include "Materials/PhongMaterial.hpp"
#include "Meshes/CylinderMesh.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"
using namespace mare;

// Standard Library
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>


// GDAL
#include <ogrsf_frmts.h>

// 3DH
#include "GDAL/gdal_io.hpp"
using namespace gdal_input;

// forward declarations
class HydraulicLink;

/**
 * @brief A Hydraulic Node in a Hydraulic Network.
 * @details Hydraulic Nodes have some number of links connected to them and have
 * a constant head at steady state. Hydraulic Links are used to calculate the
 * headloss between Hydraulic Nodes.
 * @see HydraulicLink
 *
 */
class HydraulicNode {
public:
  HydraulicNode() {}
  virtual ~HydraulicNode() {}
  inline void set_ID(std::string str) { uID_ = str; }
  inline void set_x(double x) { x_ = x; }
  inline void set_y(double y) { y_ = y; }
  inline void set_z(double z) { z_ = z; }
  inline std::string get_ID() { return uID_; }
  inline double get_x() const { return x_; }
  inline double get_y() const { return y_; }
  inline double get_z() const { return z_; }
  inline void push_link(Referenced<HydraulicLink> link) {
    links_.push_back(link);
  }
  inline void pull_link(Referenced<HydraulicLink> const &link) {
    auto it = std::find(links_.begin(), links_.end(), link);
    if (it != links_.end()) {
      links_.erase(it);
    }
  }
  inline std::vector<Referenced<HydraulicLink>> const &get_links() {
    return links_;
  }

private:
  std::string uID_{}; /**< The unique ID of the HydraulicNode.*/
  double x_ = 0.0; /**< The Easting coordinate in the node's coordinate system
                read from file.*/
  double y_ = 0.0; /**< The Northing coordinate in the node's coordinate system
                read from file.*/
  double z_ = 0.0; /**< The elevation coordinate in the node's coordinate system
read from file.*/
  std::vector<Referenced<HydraulicLink>>
      links_{}; /**< The HydraulicLinks connected to the HydraulicNode.*/
};

/**
 * @brief A Manhole is a type of Hydraulic Node in a Hydraulic Network.
 *
 */
class Manhole : public HydraulicNode {
public:
  Manhole() {}
  inline void set_invert(double invert) { set_z(invert); }
  inline void set_diameter(double diameter) { diameter_ = diameter; }
  inline void set_depth(double depth) { depth_ = depth; }
  inline double get_invert() const { return get_z(); }
  inline double get_diameter() const { return diameter_; }
  inline double get_depth() const { return depth_; }

private:
  double diameter_ = 0.0; /**< The inner diameter of the Manhole.*/
  double depth_ =
      0.0; /**< The depth of the Manhole from the invert to the surface.*/
};

/**
 * @brief A Hydraulic Link in a Hydraulic Network.
 * @details Hydraulic Links connect two Hydraulic Nodes and contain the
 * information used to calcualte the headloss between the connected Hydraulic
 * Nodes.
 * @see HydraulicNode
 */
class HydraulicLink {
public:
  HydraulicLink() {}
  inline void set_ID(std::string str) { uID_ = str; }
  inline void set_up_node(Referenced<HydraulicNode> node) { up_node_ = node; }
  inline void set_dn_node(Referenced<HydraulicNode> node) { dn_node_ = node; }
  inline std::string get_ID() { return uID_; }
  inline HydraulicNode *get_up_node() { return up_node_.get(); }
  inline HydraulicNode *get_dn_node() { return dn_node_.get(); }
  virtual ~HydraulicLink() {}

private:
  std::string uID_{}; /**< The unique ID of the HydraulicLink.*/
  Referenced<HydraulicNode> up_node_ =
      nullptr; /**< The upstream HydraulicNode of the HydraulicLink.*/
  Referenced<HydraulicNode> dn_node_ =
      nullptr; /**< The downstream HydraulicNode of the HydraulicLink.*/
};

/**
 * @brief A HydraulicLink for a Pipe element in a HydraulicNetwork.
 *
 */
class Pipe : public HydraulicLink {
public:
  Pipe() {}
  inline void set_updrop(double drop) { updrop_ = drop; }
  inline void set_dndrop(double drop) { dndrop_ = drop; }
  inline void set_dia(double dia) { dia_ = dia; }
  inline double get_updrop() { return updrop_; }
  inline double get_dndrop() { return dndrop_; }
  inline double get_dia() { return dia_; }

private:
  double updrop_ = 0.0; /**< The drop from the invert of the upstream manhole to
                     the invert of the upstream end of the pipe.*/
  double dndrop_ = 0.0; /**< The drop from the invert of the downstream manhole
                     to the invert of the downstream end of the pipe.*/
  double dia_ = 0.0;    /**< The inner diameter of the pipe.*/
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
class HydraulicNetwork : public RenderPack {
public:
  HydraulicNetwork();
  void import_manholes(VectorDataset *mh_dataset, std::string layer_name,
                       std::string const &uID_field,
                       std::string const &invert_field,
                       std::string const &diameter_field,
                       std::string const &depth_field);
  void import_pipes(VectorDataset *pipe_dataset, std::string layer_name,
                    std::string const &upID_field,
                    std::string const &dnID_field,
                    std::string const &updrop_field,
                    std::string const &dndrop_field,
                    std::string const &dia_field);
  void construct_pipe_meshes();

  void construct_manhole_meshes();
  glm::dvec3 calc_offset();
  static Referenced<HydraulicNetwork> LoadedNetwork;

private:
  float horizontal_exaggeration = 1.0f;
  float vertical_exaggeration = 1.0f;
  Referenced<InstancedMesh> manholes;
  Referenced<CylinderMesh> manhole_mesh;
  Referenced<PhongMaterial> manhole_material;
  Referenced<InstancedMesh> pipes;
  Referenced<CylinderMesh> pipe_mesh;
  Referenced<PhongMaterial> pipe_material;
  std::unordered_map<std::string, Referenced<HydraulicNode>>
      nodes_; /**< The HydraulicNodes in the HydraulicNetwork.*/
  std::unordered_map<std::string, Referenced<HydraulicLink>>
      links_; /**< The HydraulicLinks in the HydraulicNetwork.*/
};

#endif