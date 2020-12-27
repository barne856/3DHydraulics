#include "Entities/RibbonTools/NodeTool.hpp"
#include "Entities/HydraulicNetwork.hpp"
#include "Entities/UI/RibbonUI.hpp"
#include "Meshes/CircleMesh.hpp"
#include "Scene.hpp"
#include "Systems/Controls/OrbitControls.hpp"

NodeTool::NodeTool(Layer *layer) : RibbonTool(layer) {
  // The Tool's Icon
  icon = gen_ref<InstancedMesh>(3);
  auto node_mesh = gen_ref<CircleMesh>(12, 0.125f);
  std::dynamic_pointer_cast<InstancedMesh>(icon)->set_mesh(node_mesh);
  std::dynamic_pointer_cast<InstancedMesh>(icon)->push_instance(
      glm::translate(glm::mat4(1.0f), {0.25f, 0.25f, 0.0f}));
  std::dynamic_pointer_cast<InstancedMesh>(icon)->push_instance(
      glm::translate(glm::mat4(1.0f), {-0.125f, 0.125f, 0.0f}));
  std::dynamic_pointer_cast<InstancedMesh>(icon)->push_instance(
      glm::translate(glm::mat4(1.0f), {-0.25f, -0.25f, 0.0f}));
  // generate and push controls for the tool onto the stack
  gen_system<NodeToolControls>();
}

void NodeTool::on_deselect() {
  Renderer::set_cursor(CursorType::ARROW);
  auto sys = (Renderer::get_info().scene)->get_system<OrbitControls>();
  sys ? sys->left_click_disabled = false : false;
}

void NodeTool::init_flyout_elements(uint32_t ribbon_width) {
  ribbon_width_in_pixels = ribbon_width;
  // Root Flyout
  util::Rect bounds = util::Rect();
  root_guide = gen_ref<FlyoutGuide<RibbonTool>>(base_layer, "NODES");
  root_guide->back_button->set_on_click_callback(
      RibbonTool::close_flyout_callback, this);
  import_flyout_button =
      gen_ref<Button<NodeTool>>(base_layer, bounds, "IMPORT");
  import_flyout_button->set_on_click_callback(open_import_flyout, this);
  edit_flyout_button = gen_ref<Button<NodeTool>>(base_layer, bounds, "EDIT");
  edit_flyout_button->set_on_click_callback(open_edit_flyout, this);
  // Import Flyout
  import_guide = gen_ref<FlyoutGuide<NodeTool>>(base_layer, "IMPORT");
  import_guide->back_button->set_on_click_callback(open_root_flyout, this);
  file_browser =
      gen_ref<BrowseFile<NodeTool>>(base_layer, bounds, "FILE: ", "shp");
  file_browser->set_on_file_select_callback(on_file_select, this);
  layer_selection = gen_ref<ImportSelection<NodeTool>>(base_layer, "LAYER: ");
  layer_selection->field_dropdown->set_on_select_callback(on_layer_select,
                                                          this);
  ID_selection = gen_ref<ImportSelection<NodeTool>>(base_layer, "ID: ");
  shape_selection = gen_ref<ImportSelection<NodeTool>>(base_layer, "SHAPE: ");
  d1_selection = gen_ref<ImportSelection<NodeTool>>(base_layer, "D1: ", true);
  d2_selection = gen_ref<ImportSelection<NodeTool>>(base_layer, "D2: ", true);
  invert_selection =
      gen_ref<ImportSelection<NodeTool>>(base_layer, "INVERT: ", true);
  depth_selection =
      gen_ref<ImportSelection<NodeTool>>(base_layer, "DEPTH: ", true);
  import_execute_button =
      gen_ref<Button<NodeTool>>(base_layer, bounds, "IMPORT");
  import_execute_button->set_on_click_callback(import_nodes, this);
  open_root_flyout(this);
  // Edit Flyout
  edit_guide = gen_ref<FlyoutGuide<NodeTool>>(base_layer, "EDIT");
  edit_guide->back_button->set_on_click_callback(open_root_flyout, this);
  select_button = gen_ref<Button<NodeTool>>(base_layer, bounds, "SELECT");
  create_button = gen_ref<Button<NodeTool>>(base_layer, bounds, "CREATE");
  create_button->set_on_click_callback(open_create_flyout, this);
  move_button = gen_ref<Button<NodeTool>>(base_layer, bounds, "MOVE");
  // Create Flyout
  create_guide = gen_ref<FlyoutGuide<NodeTool>>(base_layer, "CREATE");
  create_guide->back_button->set_on_click_callback(open_edit_flyout, this);
  create_shape_selection =
      gen_ref<ImportSelection<NodeTool>>(base_layer, "SHAPE: ");
  create_shape_selection->set_field_dropdown_selection_options(
      {"CIRCLE", "RECTANGLE"});
  create_shape_selection->field_dropdown->set_on_select_callback(
      on_create_shape_select, this);
  create_diameter_input_box =
      gen_ref<InputBox<NodeTool>>(base_layer, "DIA: ", true);
  create_width_input_box =
      gen_ref<InputBox<NodeTool>>(base_layer, "WIDTH: ", true);
  create_length_input_box =
      gen_ref<InputBox<NodeTool>>(base_layer, "LENGTH: ", true);
  create_depth_input_box =
      gen_ref<InputBox<NodeTool>>(base_layer, "DEPTH: ", true);
  create_elev_input_box =
      gen_ref<InputBox<NodeTool>>(base_layer, "ELEV: ", true);
  create_ID_input_box = gen_ref<InputBox<NodeTool>>(base_layer, "ID: ");
}
// Flyout Callbacks
void NodeTool::open_root_flyout(NodeTool *tool) {
  tool->clear_flyout_elements();
  // Push Root Flyout Elements
  tool->push_flyout_element(tool->root_guide,
                            {tool->ribbon_width_in_pixels,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->import_flyout_button,
                            {tool->ribbon_width_in_pixels,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->edit_flyout_button,
                            {tool->ribbon_width_in_pixels,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->rescale(tool->ribbon_width_world);
}
void NodeTool::open_import_flyout(NodeTool *tool) {
  tool->clear_flyout_elements();
  // Push Import Flyout Elements
  tool->push_flyout_element(tool->import_guide,
                            {tool->ribbon_width_in_pixels,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->file_browser,
                            {tool->ribbon_width_in_pixels / 2,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->layer_selection,
                            {tool->ribbon_width_in_pixels / 2,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->ID_selection,
                            {tool->ribbon_width_in_pixels / 2,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->shape_selection,
                            {tool->ribbon_width_in_pixels / 2,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->d1_selection,
                            {tool->ribbon_width_in_pixels / 2,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->d2_selection,
                            {tool->ribbon_width_in_pixels / 2,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->invert_selection,
                            {tool->ribbon_width_in_pixels / 2,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->depth_selection,
                            {tool->ribbon_width_in_pixels / 2,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->push_flyout_element(tool->import_execute_button,
                            {tool->ribbon_width_in_pixels,
                             tool->ribbon_width_in_pixels / 5,
                             tool->ribbon_width_in_pixels / 10});
  tool->rescale(tool->ribbon_width_world);
}
void NodeTool::open_edit_flyout(NodeTool *tool) {
  tool->clear_flyout_elements();
  glm::ivec3 standard_slot = {tool->ribbon_width_in_pixels,
                              tool->ribbon_width_in_pixels / 5,
                              tool->ribbon_width_in_pixels / 10};
  // Push Edit Flyout Elements
  tool->push_flyout_element(tool->edit_guide, standard_slot);
  tool->push_flyout_element(tool->select_button, standard_slot);
  tool->push_flyout_element(tool->create_button, standard_slot);
  tool->push_flyout_element(tool->move_button, standard_slot);
  tool->rescale(tool->ribbon_width_world);
}
// Import callback
void NodeTool::on_file_select(NodeTool *tool) {
  tool->imported_nodes = gen_ref<VectorDataset>(tool->file_browser->file_name);
  tool->layer_selection->set_field_dropdown_selection_options(
      tool->imported_nodes->read_layer_names());
  if (tool->layer_selection->field_dropdown->get_value().empty()) {
    tool->ID_selection->set_field_dropdown_selection_options({});
    tool->shape_selection->set_field_dropdown_selection_options({});
    tool->d1_selection->set_field_dropdown_selection_options({});
    tool->d1_selection->set_unit_dropdown_selection_options({});
    tool->d2_selection->set_field_dropdown_selection_options({});
    tool->d2_selection->set_unit_dropdown_selection_options({});
    tool->invert_selection->set_field_dropdown_selection_options({});
    tool->invert_selection->set_unit_dropdown_selection_options({});
    tool->depth_selection->set_field_dropdown_selection_options({});
    tool->depth_selection->set_unit_dropdown_selection_options({});
  }
}
void NodeTool::on_layer_select(NodeTool *tool) {
  tool->ID_selection->set_field_dropdown_selection_options(
      tool->imported_nodes->read_field_names(
          tool->layer_selection->field_dropdown->get_value()));
  tool->shape_selection->set_field_dropdown_selection_options(
      tool->imported_nodes->read_field_names(
          tool->layer_selection->field_dropdown->get_value()));
  tool->d1_selection->set_field_dropdown_selection_options(
      tool->imported_nodes->read_field_names(
          tool->layer_selection->field_dropdown->get_value()));
  tool->d1_selection->set_unit_dropdown_selection_options({"m", "ft", "in"});
  tool->d2_selection->set_field_dropdown_selection_options(
      tool->imported_nodes->read_field_names(
          tool->layer_selection->field_dropdown->get_value()));
  tool->d2_selection->set_unit_dropdown_selection_options({"m", "ft", "in"});
  tool->invert_selection->set_field_dropdown_selection_options(
      tool->imported_nodes->read_field_names(
          tool->layer_selection->field_dropdown->get_value()));
  tool->invert_selection->set_unit_dropdown_selection_options(
      {"m", "ft", "in"});
  tool->depth_selection->set_field_dropdown_selection_options(
      tool->imported_nodes->read_field_names(
          tool->layer_selection->field_dropdown->get_value()));
  tool->depth_selection->set_unit_dropdown_selection_options({"m", "ft", "in"});
}
void NodeTool::import_nodes(NodeTool *tool) {}

// Edit
void NodeTool::open_create_flyout(NodeTool *tool) {
  tool->clear_flyout_elements();
  glm::ivec3 standard_slot = {tool->ribbon_width_in_pixels,
                              tool->ribbon_width_in_pixels / 5,
                              tool->ribbon_width_in_pixels / 10};
  glm::ivec3 half_slot = {tool->ribbon_width_in_pixels / 2,
                          tool->ribbon_width_in_pixels / 5,
                          tool->ribbon_width_in_pixels / 10};
  // Push Edit Flyout Elements
  tool->push_flyout_element(tool->create_guide, standard_slot);
  tool->push_flyout_element(tool->create_shape_selection, half_slot);
  tool->rescale(tool->ribbon_width_world);
}

// Create
void NodeTool::on_create_shape_select(NodeTool *tool) {
  tool->clear_flyout_elements();
  glm::ivec3 standard_slot = {tool->ribbon_width_in_pixels,
                              tool->ribbon_width_in_pixels / 5,
                              tool->ribbon_width_in_pixels / 10};
  glm::ivec3 half_slot = {tool->ribbon_width_in_pixels / 2,
                          tool->ribbon_width_in_pixels / 5,
                          tool->ribbon_width_in_pixels / 10};
  // Push Create Flyout Elements
  tool->push_flyout_element(tool->create_guide, standard_slot);
  tool->push_flyout_element(tool->create_shape_selection, half_slot);
  if (tool->create_shape_selection->field_dropdown->get_value() == "CIRCLE") {
    tool->push_flyout_element(tool->create_diameter_input_box, half_slot);
  }
  if (tool->create_shape_selection->field_dropdown->get_value() ==
      "RECTANGLE") {
    tool->push_flyout_element(tool->create_width_input_box, half_slot);
    tool->push_flyout_element(tool->create_length_input_box, half_slot);
  }
  tool->push_flyout_element(tool->create_depth_input_box, half_slot);
  tool->push_flyout_element(tool->create_elev_input_box, half_slot);
  tool->push_flyout_element(tool->create_ID_input_box, half_slot);
  tool->rescale(tool->ribbon_width_world);
}

bool NodeToolControls::on_mouse_button(RendererInput const &input,
                                       NodeTool *tool) {
  if (input.LEFT_MOUSE_JUST_PRESSED && !is_over_flyout) {
    // generate new node at the clicked position
    float dia, depth, elev;
    std::string ID;
    try {
      dia = tool->create_diameter_input_box->get_input_value_as_float();
    } catch (const std::exception &e) {
      dia = 4.0f; // default value
    }
    try {
      depth = tool->create_depth_input_box->get_input_value_as_float();
    } catch (const std::exception &e) {
      depth = 8.0f; // default value
    }
    try {
      elev = tool->create_elev_input_box->get_input_value_as_float();
    } catch (const std::exception &e) {
      elev = 0.0f; // default value
    }
    try {
      ID = tool->create_ID_input_box->get_input_value_as_string();
    } catch (const std::exception &e) {
      ID = ""; // default value
    }
    glm::vec3 position = Renderer::raycast(Renderer::get_info().scene);
    HydraulicNetwork *network = HydraulicNetwork::LoadedNetwork.get();
    double easting = position.x + network->offset.x;
    double northing = position.y + network->offset.y;
    Referenced<CylinderNode> node = gen_ref<CylinderNode>();
    node->easting = easting;
    node->northing = northing;
    node->invert_elevation = elev;
    node->node_depth = depth;
    node->ID = ID;
    node->inner_diameter = dia;
    network->add_node(node);
  }
  return false;
}

bool NodeToolControls::on_mouse_move(RendererInput const &input,
                                     NodeTool *tool) {
  // Check if create flyout is open and cursor is off of flyout
  auto guides = tool->get_flyout_elements<FlyoutGuide<NodeTool>>();
  RibbonUI *ribbon = tool->base_layer->get_entity<RibbonUI>();
  glm::vec2 layer_coords = ribbon->get_model_coords();
  float scale = ribbon->get_layer()->get_ortho_scale();
  float aspect = Renderer::get_info().window_aspect;
  float ribbon_width_layer =
      scale * aspect * static_cast<float>(ribbon->ribbon_width) /
      static_cast<float>(Renderer::get_info().window_width);
  bool in_flyout = layer_coords.x < -scale * aspect + 6.0f * ribbon_width_layer;
  auto sys = (Renderer::get_info().scene)->get_system<OrbitControls>();
  if (guides.size() > 0 && guides[0] == tool->create_guide.get() &&
      tool->selected && !in_flyout) {
    Renderer::set_cursor(CursorType::CROSSHAIRS);
    sys ? sys->left_click_disabled = true : false;
    is_over_flyout = false;
  } else {
    Renderer::set_cursor(CursorType::ARROW);
    sys ? sys->left_click_disabled = false : false;
    is_over_flyout = true;
  }

  return false;
}
