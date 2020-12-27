// #include "Entities/RibbonTools/LoadTool.hpp"
// #include "Scenes/MainScene.hpp"
// 
// LoadTool::LoadTool(Layer *layer, uint32_t slot_height_in_pixels)
//     : RibbonTool(layer) {
//   icon = gen_ref<CompositeMesh>();
//   auto vertical = gen_ref<QuadrangleMesh>();
//   auto horizontal = gen_ref<QuadrangleMesh>();
//   vertical->set_scale({0.2f, 0.6f, 1.0f});
//   horizontal->set_scale({0.6f, 0.2f, 1.0f});
//   std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(vertical);
//   std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(horizontal);
//   util::Rect bounds = util::Rect();
//   if (layer) {
//     slot_height = slot_height_in_pixels; // total slot height including padding
// 
//     // data type selection dropdown box
//     data_type_dropdown =
//         gen_ref<Dropdown<LoadTool>>(layer, bounds, "DATA TYPE");
//     data_type_dropdown->set_selection_options({"MANHOLES", "PIPES", "DEM"}, 4);
//     data_type_dropdown->set_on_select_callback(on_data_type_select, this);
// 
//     // Manholes data type
//     open_mh_button =
//         gen_ref<Button<LoadTool>>(layer, bounds, "OPEN MANHOLE DATA");
//     open_mh_button->set_on_click_callback(open_manholes_dataset, this);
//     mh_layer_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "LAYER");
//     mh_layer_dropdown->set_on_select_callback(on_layer_select, this);
//     mh_ID_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "ID");
//     mh_inv_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "INV");
//     mh_dia_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "DIA");
//     mh_depth_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "DEPTH");
//     import_mh_button =
//         gen_ref<Button<LoadTool>>(layer, bounds, "IMPORT MANHOLES");
//     import_mh_button->set_on_click_callback(import_manholes, this);
// 
//     // Pipes data type
//     open_pipe_button =
//         gen_ref<Button<LoadTool>>(layer, bounds, "OPEN PIPE DATA");
//     open_pipe_button->set_on_click_callback(open_pipe_dataset, this);
//     pipe_layer_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "LAYER");
//     pipe_layer_dropdown->set_on_select_callback(on_layer_select, this);
//     pipe_upID_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "UPID");
//     pipe_dnID_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "DNID");
//     pipe_updrop_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "UPDROP");
//     pipe_dndrop_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "DNDROP");
//     pipe_dia_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "DIA");
//     import_pipe_button =
//         gen_ref<Button<LoadTool>>(layer, bounds, "IMPORT PIPES");
//     import_pipe_button->set_on_click_callback(import_pipes, this);
// 
//     // DEM data type
//     open_dem_button = gen_ref<Button<LoadTool>>(layer, bounds, "OPEN DEM DATA");
//     open_dem_button->set_on_click_callback(open_dem, this);
//     open_image_button =
//         gen_ref<Button<LoadTool>>(layer, bounds, "OPEN IMAGE DATA");
//     open_image_button->set_on_click_callback(open_image, this);
//     terrain_grid_size_dropdown =
//         gen_ref<Dropdown<LoadTool>>(layer, bounds, "GRID SIZE");
//     terrain_grid_size_dropdown->set_on_select_callback(on_grid_size_select,
//                                                        this);
//     terrain_grid_size_dropdown->set_value(std::to_string(grid_size));
//     terrain_grid_size_dropdown->set_selection_options(
//         {"4", "5", "6", "7", "8", "9", "10", "11", "12"}, 4);
//     terrain_level_count_dropdown =
//         gen_ref<Dropdown<LoadTool>>(layer, bounds, "LEVEL COUNT");
//     terrain_level_count_dropdown->set_on_select_callback(on_level_count_select,
//                                                          this);
//     terrain_level_count_dropdown->set_value(std::to_string(level_count));
//     terrain_level_count_dropdown->set_selection_options(
//         {"4", "5", "6", "7", "8", "9", "10", "11", "12"}, 4);
//     import_dem_button = gen_ref<Button<LoadTool>>(layer, bounds, "IMPORT DEM");
//     import_dem_button->set_on_click_callback(import_dem, this);
// 
//     // push data type dropdown
//     push_flyout_element(data_type_dropdown,
//                         {slot_height, slot_height / 5, slot_height / 10});
//   }
// }
// void LoadTool::on_data_type_select(LoadTool *tool) {
//   int slot_height = tool->slot_height;
//   glm::ivec3 slot_vec = {slot_height, slot_height / 5, slot_height / 10};
//   tool->clear_flyout_elements();
//   tool->push_flyout_element(tool->data_type_dropdown, slot_vec);
//   if (tool->data_type_dropdown->get_value() == "MANHOLES") {
//     tool->push_flyout_element(tool->open_mh_button, slot_vec);
//     tool->push_flyout_element(tool->mh_layer_dropdown, slot_vec);
//     tool->push_flyout_element(tool->mh_ID_dropdown, slot_vec);
//     tool->push_flyout_element(tool->mh_inv_dropdown, slot_vec);
//     tool->push_flyout_element(tool->mh_dia_dropdown, slot_vec);
//     tool->push_flyout_element(tool->mh_depth_dropdown, slot_vec);
//     tool->push_flyout_element(tool->import_mh_button, slot_vec);
//   }
//   if (tool->data_type_dropdown->get_value() == "PIPES") {
//     tool->push_flyout_element(tool->open_pipe_button, slot_vec);
//     tool->push_flyout_element(tool->pipe_layer_dropdown, slot_vec);
//     tool->push_flyout_element(tool->pipe_upID_dropdown, slot_vec);
//     tool->push_flyout_element(tool->pipe_dnID_dropdown, slot_vec);
//     tool->push_flyout_element(tool->pipe_updrop_dropdown, slot_vec);
//     tool->push_flyout_element(tool->pipe_dndrop_dropdown, slot_vec);
//     tool->push_flyout_element(tool->pipe_dia_dropdown, slot_vec);
//     tool->push_flyout_element(tool->import_pipe_button, slot_vec);
//   }
//   if (tool->data_type_dropdown->get_value() == "DEM") {
//     tool->push_flyout_element(tool->open_dem_button, slot_vec);
//     tool->push_flyout_element(tool->open_image_button, slot_vec);
//     tool->push_flyout_element(tool->terrain_grid_size_dropdown, slot_vec);
//     tool->push_flyout_element(tool->terrain_level_count_dropdown, slot_vec);
//     tool->push_flyout_element(tool->import_dem_button, slot_vec);
//   }
//   tool->rescale(tool->ribbon_width_world);
// }
// void LoadTool::on_layer_select(LoadTool *tool) {
//   if (tool->data_type_dropdown->get_value() == "MANHOLES") {
//     auto field_names = tool->manhole_dataset->read_field_names(
//         tool->mh_layer_dropdown->get_value());
//     tool->mh_ID_dropdown->set_selection_options(field_names, 4);
//     tool->mh_inv_dropdown->set_selection_options(field_names, 4);
//     tool->mh_dia_dropdown->set_selection_options(field_names, 4);
//     tool->mh_depth_dropdown->set_selection_options(field_names, 4);
//   }
//   if (tool->data_type_dropdown->get_value() == "PIPES") {
//     auto field_names = tool->pipe_dataset->read_field_names(
//         tool->pipe_layer_dropdown->get_value());
//     tool->pipe_upID_dropdown->set_selection_options(field_names, 4);
//     tool->pipe_dnID_dropdown->set_selection_options(field_names, 4);
//     tool->pipe_updrop_dropdown->set_selection_options(field_names, 4);
//     tool->pipe_dndrop_dropdown->set_selection_options(field_names, 4);
//     tool->pipe_dia_dropdown->set_selection_options(field_names, 4);
//   }
// }
// void LoadTool::open_manholes_dataset(LoadTool *tool) {
//   std::string filepath = open_file_dialog("shp");
//   if (!filepath.empty()) {
//     tool->manhole_dataset = std::make_shared<VectorDataset>(filepath);
//     if (tool->manhole_dataset) {
//       auto layer_names = tool->manhole_dataset->read_layer_names();
//       tool->mh_layer_dropdown->set_selection_options(layer_names, 4);
//     }
//   }
// }
// void LoadTool::close_manhole_dataset(LoadTool *tool) {
//   tool->mh_layer_dropdown->set_selection_options({}, 4);
//   tool->mh_ID_dropdown->set_selection_options({}, 4);
//   tool->mh_inv_dropdown->set_selection_options({}, 4);
//   tool->mh_dia_dropdown->set_selection_options({}, 4);
//   tool->mh_depth_dropdown->set_selection_options({}, 4);
//   tool->manhole_dataset = nullptr;
// }
// 
// void LoadTool::import_manholes(LoadTool *tool) {
//   if (tool->manhole_dataset) {
//     HydraulicNetwork::LoadedNetwork->import_manholes(
//         tool->manhole_dataset.get(), tool->mh_layer_dropdown->get_value(),
//         tool->mh_ID_dropdown->get_value(), tool->mh_inv_dropdown->get_value(),
//         tool->mh_dia_dropdown->get_value(),
//         tool->mh_depth_dropdown->get_value());
//     close_manhole_dataset(tool);
//   }
// }
// void LoadTool::open_pipe_dataset(LoadTool *tool) {
//   std::string filepath = open_file_dialog("shp");
//   if (!filepath.empty()) {
//     tool->pipe_dataset = std::make_shared<VectorDataset>(filepath);
//     if (tool->pipe_dataset) {
//       auto layer_names = tool->pipe_dataset->read_layer_names();
//       tool->pipe_layer_dropdown->set_selection_options(layer_names, 4);
//     }
//   }
// }
// void LoadTool::close_pipe_dataset(LoadTool *tool) {
//   tool->pipe_layer_dropdown->set_selection_options({}, 4);
//   tool->pipe_upID_dropdown->set_selection_options({}, 4);
//   tool->pipe_dnID_dropdown->set_selection_options({}, 4);
//   tool->pipe_updrop_dropdown->set_selection_options({}, 4);
//   tool->pipe_dndrop_dropdown->set_selection_options({}, 4);
//   tool->pipe_dia_dropdown->set_selection_options({}, 4);
//   tool->pipe_dataset = nullptr;
// }
// void LoadTool::import_pipes(LoadTool *tool) {
//   if (tool->pipe_dataset) {
//     HydraulicNetwork::LoadedNetwork->import_pipes(
//         tool->pipe_dataset.get(), tool->pipe_layer_dropdown->get_value(),
//         tool->pipe_upID_dropdown->get_value(),
//         tool->pipe_dnID_dropdown->get_value(),
//         tool->pipe_updrop_dropdown->get_value(),
//         tool->pipe_dndrop_dropdown->get_value(),
//         tool->pipe_dia_dropdown->get_value());
//     close_pipe_dataset(tool);
//   }
// }
// void LoadTool::open_dem(LoadTool *tool) {
//   std::string filepath = open_file_dialog("bil");
//   if (!filepath.empty()) {
//     tool->dem = gen_ref<RasterDataset>(filepath);
//   }
// }
// void LoadTool::open_image(LoadTool *tool) {
//   std::string filepath = open_file_dialog("tif");
//   if (!filepath.empty()) {
//     tool->image = gen_ref<RasterDataset>(filepath);
//   }
// }
// void LoadTool::on_grid_size_select(LoadTool *tool) {
//   tool->grid_size = std::stoi(tool->terrain_grid_size_dropdown->get_value());
// }
// void LoadTool::on_level_count_select(LoadTool *tool) {
//   tool->level_count =
//       std::stoi(tool->terrain_level_count_dropdown->get_value());
// }
// void LoadTool::import_dem(LoadTool *tool) {
//   if (!tool->dem_terrain) {
//     auto main_scene = dynamic_cast<MainScene *>(Renderer::get_info().scene);
//     if (!main_scene) {
//       // MainScene is not active
//       return;
//     }
//     auto offset = main_scene->get_world_offset();
//     if (tool->dem && tool->image) {
//       // generate and push terrain
//       tool->dem_terrain = gen_ref<Terrain>(tool->dem.get(), tool->image.get(),
//                                            tool->grid_size, tool->level_count);
//       main_scene->push_entity(tool->dem_terrain);
//     }
//   }
// }
// void LoadTool::on_select() {}
// void LoadTool::on_deselect() {
//   if (manhole_dataset) {
//     manhole_dataset = nullptr;
//     mh_ID_dropdown->set_selection_options({}, 4);
//     mh_inv_dropdown->set_selection_options({}, 4);
//     mh_dia_dropdown->set_selection_options({}, 4);
//     mh_depth_dropdown->set_selection_options({}, 4);
//   }
//   if (pipe_dataset) {
//     pipe_dataset = nullptr;
//     pipe_upID_dropdown->set_selection_options({}, 4);
//     pipe_dnID_dropdown->set_selection_options({}, 4);
//     pipe_updrop_dropdown->set_selection_options({}, 4);
//     pipe_dndrop_dropdown->set_selection_options({}, 4);
//     pipe_dia_dropdown->set_selection_options({}, 4);
//   }
// }