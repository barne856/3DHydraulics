#ifndef HYDRAULICNETWORK
#define HYDRAULICNETWORK

// MARE
#include "Components/RenderPack.hpp"
#include "Mare.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"
using namespace mare;

// Standard Library
#include <string>
#include <vector>

// GDAL
#include <ogrsf_frmts.h>

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

private:
  std::string uID_; /**< The unique ID of the HydraulicNode.*/
  double x_; /**< The Easting coordinate in the node's coordinate system read
                from file.*/
  double y_; /**< The Northing coordinate in the node's coordinate system read
                from file.*/
  std::vector<Referenced<HydraulicLink>>
      links_; /**< The HydraulicLinks connected to the HydraulicNode.*/
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
  virtual ~HydraulicLink() {}

private:
  std::string uID_; /**< The unique ID of the HydraulicLink.*/
  Referenced<HydraulicNode>
      up_node_; /**< The upstream HydraulicNode of the HydraulicLink.*/
  Referenced<HydraulicNode>
      dn_node_; /**< The downstream HydraulicNode of the HydraulicLink.*/
};

/**
 * @brief A Hydraulic Network class that represents a newtork of Hydraulic Nodes
 * and Hydraulic Links.
 * @details The Hydraulic Network contains all of the information on the
 * geometry of the hydraulic components in the network and is used to render the
 * network. The Hydraulic Network is a completely static class.
 * @see HydraulicNode
 * @see HydraulicLink
 */
class HydraulicNetwork : public RenderPack {
public:
  HydraulicNetwork() { gen_system<PacketRenderer>(); }
  static void import_manholes(OGRLayer *node_layer,
                              std::string const &uID_field,
                              std::string const &invert_field,
                              std::string const &diameter_field,
                              std::string const &depth_field) {
    // for each point geometry in the layer, construct a new Manhole and
    // push it onto the nodes_ vector.

    // construct an InstancedMesh to render the manholes in the Network's
    // coordinate system (offset by some amount to reduce floating point
    // rounding errors).
  }

private:
  static std::vector<Referenced<HydraulicNode>>
      nodes_; /**< The HydraulicNodes in the HydraulicNetwork.*/
  static std::vector<Referenced<HydraulicLink>>
      links_;       /**< The HydraulicLinks in the HydraulicNetwork.*/
  double x_offset_; /**< x coordinate offset from the HydraulicNode's coordinate
                       system to reduce errors from 32-bit floating point
                       rounding.*/
  double y_offset_; /**< y coordinate offset from the HydraulicNode's coordinate
                       system to reduce errors from 32-bit floating point
                       rounding.*/
};

/**
 * @brief A Manhole is a type of Hydraulic Node in a Hydraulic Network.
 *
 */
class Manhole : public HydraulicNode {
public:
  Manhole() {}

private:
  float invert_;   /**< The invert elevation of the Manhole.*/
  float diameter_; /**< The inner diameter of the Manhole.*/
  float depth_; /**< The depth of the Manhole from the invert to the surface.*/
};

#endif