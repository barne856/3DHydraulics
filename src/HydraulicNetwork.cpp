#include "Entities/HydraulicNetwork.hpp"
#include "Scenes/MainScene.hpp"

Referenced<HydraulicNetwork> HydraulicNetwork::LoadedNetwork = nullptr;

HydraulicNetwork::HydraulicNetwork() {
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
void HydraulicNetwork::import_manholes(VectorDataset *mh_dataset,
                                       std::string layer_name,
                                       std::string const &uID_field,
                                       std::string const &invert_field,
                                       std::string const &diameter_field,
                                       std::string const &depth_field) {
  nodes_.clear();
  manholes->clear_instances();
  if (mh_dataset) {
    for (int64_t i = 0; i < mh_dataset->get_feature_count(layer_name); i++) {
      Referenced<Manhole> mh = gen_ref<Manhole>();
      glm::dvec3 point = mh_dataset->get_point_feature_geometry(layer_name, i);
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
void HydraulicNetwork::import_pipes(VectorDataset *pipe_dataset,
                                    std::string layer_name,
                                    std::string const &upID_field,
                                    std::string const &dnID_field,
                                    std::string const &updrop_field,
                                    std::string const &dndrop_field,
                                    std::string const &dia_field) {
  links_.clear();
  pipes->clear_instances();
  if (pipe_dataset) {
    for (int64_t i = 0; i < pipe_dataset->get_feature_count(layer_name); i++) {
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
void HydraulicNetwork::construct_pipe_meshes() {
  pipes->clear_instances();
  auto main_scene = dynamic_cast<MainScene*>(Renderer::get_info().scene);
  glm::dvec3 offset{};
  if (main_scene) {
    offset = main_scene->get_world_offset();
  } else {
    // MainScene is not active
    return;
  }
  for (auto const &link : links_) {
    if (auto const &pipe = std::dynamic_pointer_cast<Pipe>(link.second)) {
      auto up_node = pipe->get_up_node();
      auto dn_node = pipe->get_dn_node();
      if (up_node && dn_node) {
        glm::vec3 upstream_point{
            static_cast<float>(pipe->get_up_node()->get_x() - offset.x),
            static_cast<float>(pipe->get_up_node()->get_y() - offset.y),
            vertical_exaggeration *
                static_cast<float>(pipe->get_up_node()->get_z() - offset.z +
                                   pipe->get_updrop())};
        glm::vec3 downstream_point{
            static_cast<float>(pipe->get_dn_node()->get_x() - offset.x),
            static_cast<float>(pipe->get_dn_node()->get_y() - offset.y),
            vertical_exaggeration *
                static_cast<float>(pipe->get_dn_node()->get_z() - offset.z +
                                   pipe->get_dndrop())};
        float L = glm::length(upstream_point - downstream_point);
        float D = pipe->get_dia() * horizontal_exaggeration;
        glm::vec2 horizontal_vec =
            glm::vec2(downstream_point) - glm::vec2(upstream_point);
        float X = glm::length(horizontal_vec);
        float slope_in_rads = atan2f(downstream_point.z - upstream_point.z, X);
        float phi = atan2f(horizontal_vec.y, horizontal_vec.x);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), {D, D, L});
        glm::mat4 slope =
            glm::rotate(glm::mat4(1.0f), slope_in_rads - math::PI / 2.0f,
                        {0.0f, -1.0f, 0.0f});
        glm::mat4 rotation =
            glm::rotate(glm::mat4(1.0f), phi, {0.0f, 0.0f, 1.0f});
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), upstream_point);
        glm::mat4 transform = translation * rotation * slope * scale;
        pipes->push_instance(transform);
      }
    }
  }
}

void HydraulicNetwork::construct_manhole_meshes() {
  auto main_scene = dynamic_cast<MainScene*>(Renderer::get_info().scene);
  if (main_scene) {
    if (std::isnan(main_scene->get_world_offset().x)) {
      main_scene->set_world_offset(calc_offset());
    }
  } else {
    // MainScene is not active
    return;
  }
  auto offset = main_scene->get_world_offset();
  manholes->clear_instances();
  for (auto const &node : nodes_) {
    if (auto const &mh = std::dynamic_pointer_cast<Manhole>(node.second)) {
      glm::vec3 scale = {horizontal_exaggeration * mh->get_diameter() / 12.0f,
                         horizontal_exaggeration * mh->get_diameter() / 12.0f,
                         vertical_exaggeration * mh->get_depth()};
      glm::vec3 position = {
          static_cast<float>(mh->get_x() - offset.x),
          static_cast<float>(mh->get_y() - offset.y),
          vertical_exaggeration *
              static_cast<float>(mh->get_invert() - offset.z)};
      glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                            glm::scale(glm::mat4(1.0f), scale);
      manholes->push_instance(transform);
    }
  }
}
glm::dvec3 HydraulicNetwork::calc_offset() {
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