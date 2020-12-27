// #ifndef LOADTOOL
// #define LOADTOOL
// 
// #include "Entities/RibbonTool.hpp"
// #include "Entities/UI/Button.hpp"
// #include "Entities/UI/ColorPicker.hpp"
// #include "Entities/UI/Dropdown.hpp"
// #include "Entities/UI/Slider.hpp"
// #include "Entities/UI/Switch.hpp"
// #include "Entities/UI/TextBox.hpp"
// #include "Meshes/QuadrangleMesh.hpp"
// #include "Renderer.hpp"
// using namespace mare;
// 
// #include <iostream>
// 
// // GDAL
// #include <ogrsf_frmts.h>
// 
// // 3DH
// #include "Entities/Terrain.hpp"
// #include "GDAL/gdal_io.hpp"
// using namespace gdal_input;
// 
// /**
//  * @brief A Tool used on the RibbonUI to load data into the Scene.
//  *
//  */
// class LoadTool : public RibbonTool {
// public:
//   /**
//    * @brief Construct a new LoadTool object
//    * @details The LoadTool is to be constructed inside the RibbonUI and pushed
//    * onto the tool stack.
//    * @param layer The layer which the UIElements will use to draw on.
//    * @param slot_height_in_pixels The slot height in pixels, normally the same
//    * as the ribbon width in pixels.
//    */
//   LoadTool(Layer *layer, uint32_t slot_height_in_pixels);
//   static void on_data_type_select(LoadTool *tool);
//   static void on_layer_select(LoadTool *tool);
//   static void open_manholes_dataset(LoadTool *tool);
//   static void close_manhole_dataset(LoadTool *tool);
// 
//   static void import_manholes(LoadTool *tool);
//   static void open_pipe_dataset(LoadTool *tool);
//   static void close_pipe_dataset(LoadTool *tool);
//   static void import_pipes(LoadTool *tool);
//   static void open_dem(LoadTool *tool);
//   static void open_image(LoadTool *tool);
//   static void on_grid_size_select(LoadTool *tool);
//   static void on_level_count_select(LoadTool *tool);
//   static void import_dem(LoadTool *tool);
//   void on_select() override;
//   void on_deselect() override;
// 
//   int slot_height;
//   Referenced<Dropdown<LoadTool>> data_type_dropdown;
//   // Manholes
//   std::shared_ptr<VectorDataset> manhole_dataset = nullptr;
//   Referenced<Button<LoadTool>> open_mh_button;
//   Referenced<Dropdown<LoadTool>> mh_layer_dropdown;
//   Referenced<Dropdown<LoadTool>> mh_ID_dropdown;
//   Referenced<Dropdown<LoadTool>> mh_inv_dropdown;
//   Referenced<Dropdown<LoadTool>> mh_dia_dropdown;
//   Referenced<Dropdown<LoadTool>> mh_depth_dropdown;
//   Referenced<Button<LoadTool>> import_mh_button;
//   // Pipes
//   std::shared_ptr<VectorDataset> pipe_dataset = nullptr;
//   Referenced<Button<LoadTool>> open_pipe_button;
//   Referenced<Dropdown<LoadTool>> pipe_layer_dropdown;
//   Referenced<Dropdown<LoadTool>> pipe_upID_dropdown;
//   Referenced<Dropdown<LoadTool>> pipe_dnID_dropdown;
//   Referenced<Dropdown<LoadTool>> pipe_updrop_dropdown;
//   Referenced<Dropdown<LoadTool>> pipe_dndrop_dropdown;
//   Referenced<Dropdown<LoadTool>> pipe_dia_dropdown;
//   Referenced<Button<LoadTool>> import_pipe_button;
//   // DEM
//   Referenced<RasterDataset> dem = nullptr;
//   Referenced<RasterDataset> image = nullptr;
//   Referenced<Terrain> dem_terrain = nullptr;
//   Referenced<Button<LoadTool>> open_dem_button;
//   Referenced<Button<LoadTool>> open_image_button;
//   Referenced<Dropdown<LoadTool>> terrain_grid_size_dropdown;
//   Referenced<Dropdown<LoadTool>> terrain_level_count_dropdown;
//   int grid_size = 8;
//   int level_count = 6;
//   Referenced<Button<LoadTool>> import_dem_button;
// };
// 
// #endif