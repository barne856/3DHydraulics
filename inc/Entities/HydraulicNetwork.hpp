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
  HydraulicNetwork() {
    uint32_t max_manholes = 10000;
    uint32_t max_pipes = 10000;
    manhole_mesh = gen_ref<CylinderMesh>(0.0f, math::TAU, 32);
    pipe_mesh = gen_ref<CylinderMesh>(0.0f, math::TAU, 32);
    manholes = gen_ref<InstancedMesh>(max_manholes);
    manholes->set_mesh(manhole_mesh);
    pipes = gen_ref<InstancedMesh>(max_pipes);
    pipes->set_mesh(pipe_mesh);
    manhole_material = gen_ref<PhongMaterial>();
    pipe_material = gen_ref<PhongMaterial>();
    auto light = gen_ref<Spotlight>();
    light->set_position({10.0f, 10.0f, 10.0f});
    manhole_material->set_light(light);
    pipe_material->set_light(light);
    push_packet({manholes, manhole_material});
    push_packet({pipes, pipe_material});
    gen_system<PacketRenderer>();
  }
  void import_manholes(VectorDataset *mh_dataset, std::string layer_name,
                       std::string const &uID_field,
                       std::string const &invert_field,
                       std::string const &diameter_field,
                       std::string const &depth_field) {
    nodes_.clear();
    manholes->clear_instances();
    if (mh_dataset) {
      for (int64_t i = 0; i < mh_dataset->get_feature_count(layer_name); i++) {
        Referenced<Manhole> mh = gen_ref<Manhole>();
        glm::dvec3 point =
            mh_dataset->get_point_feature_geometry(layer_name, i);
        mh->set_x(point.x);
        mh->set_y(point.y);
        mh->set_ID(mh_dataset->get_field_as_string(layer_name, i, uID_field));
        mh->set_invert(
            mh_dataset->get_field_as_double(layer_name, i, invert_field));
        mh->set_diameter(
            mh_dataset->get_field_as_double(layer_name, i, diameter_field));
        mh->set_depth(
            mh_dataset->get_field_as_double(layer_name, i, depth_field));
        nodes_.insert({mh->get_ID(), mh});
      }
      // construct an InstancedMesh to render the manholes in the Network's
      // coordinate system (offset by some amount to reduce floating point
      // rounding errors).
      construct_manhole_meshes();
    }
  }
  void import_pipes(VectorDataset *pipe_dataset, std::string layer_name,
                    std::string const &upID_field,
                    std::string const &dnID_field,
                    std::string const &updrop_field,
                    std::string const &dndrop_field,
                    std::string const &dia_field) {
    links_.clear();
    pipes->clear_instances();
    if (pipe_dataset) {
      for (int64_t i = 0; i < pipe_dataset->get_feature_count(layer_name);
           i++) {
        Referenced<Pipe> pipe = gen_ref<Pipe>();
        std::string upID =
            pipe_dataset->get_field_as_string(layer_name, i, upID_field);
        std::string dnID =
            pipe_dataset->get_field_as_string(layer_name, i, dnID_field);
        pipe->set_ID(upID + "-" + dnID);
        pipe->set_up_node(nodes_[upID]);
        pipe->set_dn_node(nodes_[dnID]);
        pipe->set_updrop(
            pipe_dataset->get_field_as_double(layer_name, i, updrop_field));
        pipe->set_dndrop(
            pipe_dataset->get_field_as_double(layer_name, i, dndrop_field));
        pipe->set_dia(
            pipe_dataset->get_field_as_double(layer_name, i, dia_field));
        links_.insert({pipe->get_ID(), pipe});
      }
      construct_pipe_meshes();
    }
  }
  void construct_pipe_meshes() {
    pipes->clear_instances();
    for (auto const &link : links_) {
      if (auto const &pipe = std::dynamic_pointer_cast<Pipe>(link.second)) {
        auto up_node = pipe->get_up_node();
        auto dn_node = pipe->get_dn_node();
        if (up_node && dn_node) {
          glm::vec3 upstream_point{
              static_cast<float>(pipe->get_up_node()->get_x() - x_offset_),
              static_cast<float>(pipe->get_up_node()->get_y() - y_offset_),
              vertical_exaggeration *
                  static_cast<float>(pipe->get_up_node()->get_z() - z_offset_ +
                                     pipe->get_updrop())};
          glm::vec3 downstream_point{
              static_cast<float>(pipe->get_dn_node()->get_x() - x_offset_),
              static_cast<float>(pipe->get_dn_node()->get_y() - y_offset_),
              vertical_exaggeration *
                  static_cast<float>(pipe->get_dn_node()->get_z() - z_offset_ +
                                     pipe->get_dndrop())};
          float L = glm::length(upstream_point - downstream_point);
          float D = pipe->get_dia() * horizontal_exaggeration;
          glm::vec2 horizontal_vec =
              glm::vec2(downstream_point) - glm::vec2(upstream_point);
          float X = glm::length(horizontal_vec);
          float slope_in_rads =
              atan2f(downstream_point.z - upstream_point.z, X);
          float phi = atan2f(horizontal_vec.y, horizontal_vec.x);
          glm::mat4 scale = glm::scale(glm::mat4(1.0f), {D, D, L});
          glm::mat4 slope =
              glm::rotate(glm::mat4(1.0f), slope_in_rads - math::PI / 2.0f,
                          {0.0f, -1.0f, 0.0f});
          glm::mat4 rotation =
              glm::rotate(glm::mat4(1.0f), phi, {0.0f, 0.0f, 1.0f});
          glm::mat4 translation =
              glm::translate(glm::mat4(1.0f), upstream_point);
          glm::mat4 transform = translation * rotation * slope * scale;
          pipes->push_instance(transform);
        }
      }
    }
  }

  void construct_manhole_meshes() {
    auto offset = calc_offset();
    x_offset_ = offset.x;
    y_offset_ = offset.y;
    z_offset_ = offset.z;
    manholes->clear_instances();
    for (auto const &node : nodes_) {
      if (auto const &mh = std::dynamic_pointer_cast<Manhole>(node.second)) {
        glm::vec3 scale = {horizontal_exaggeration * mh->get_diameter() / 12.0f,
                           horizontal_exaggeration * mh->get_diameter() / 12.0f,
                           vertical_exaggeration * mh->get_depth()};
        glm::vec3 position = {
            static_cast<float>(mh->get_x() - x_offset_),
            static_cast<float>(mh->get_y() - y_offset_),
            vertical_exaggeration *
                static_cast<float>(mh->get_invert() - z_offset_)};
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), scale);
        manholes->push_instance(transform);
      }
    }
  }
  glm::dvec3 get_offset() { return {x_offset_, y_offset_, z_offset_}; }
  glm::dvec3 calc_offset() {
    double i = 0.0;
    glm::dvec3 offset{};
    for (auto &node : nodes_) {
      offset.x += node.second->get_x();
      offset.y += node.second->get_y();
      offset.z += node.second->get_z();
      i += 1.0;
    }
    if (i > 0.0) {
      return offset / i;
    }
    return offset;
  }
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
      links_;              /**< The HydraulicLinks in the HydraulicNetwork.*/
  double x_offset_ = 0.0f; /**< x coordinate offset from the HydraulicNode's
                       coordinate system to reduce errors from 32-bit floating
                       point rounding.*/
  double y_offset_ = 0.0f; /**< y coordinate offset from the HydraulicNode's
                       coordinate system to reduce errors from 32-bit floating
                       point rounding.*/
  double z_offset_ = 0.0f; /**< z coordinate offset from the HydraulicNode's
                        coordinate system to reduce errors from 32-bit floating
                        point rounding.*/
};

#endif