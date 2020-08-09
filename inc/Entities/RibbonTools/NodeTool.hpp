#ifndef NODETOOL
#define NODETOOL

// 3DH
#include "Entities/RibbonTool.hpp"
#include "Entities/UI/BrowseFile.hpp"
#include "Entities/UI/ImportSelection.hpp"

// MARE
#include "Entities/UI/Button.hpp"
#include "Entities/UI/FlyoutGuide.hpp"
#include "Entities/UI/InputBox.hpp"

using namespace gdal_input;

class NodeTool : public RibbonTool {
public:
  NodeTool(Layer *layer);
  void on_select() override {}
  void on_deselect() override {}
  void init_flyout_elements(uint32_t ribbon_width) override;
  // Flyouts
  static void open_root_flyout(NodeTool *tool);
  static void open_import_flyout(NodeTool *tool);
  static void open_edit_flyout(NodeTool *tool);
  // Import
  static void on_file_select(NodeTool *tool);
  static void on_layer_select(NodeTool *tool);
  static void import_nodes(NodeTool *tool);
  // Edit
  static void open_create_flyout(NodeTool* tool);
  // Create
  static void on_create_shape_select(NodeTool* tool);

  // Import node dataset
  Referenced<VectorDataset> imported_nodes;
  // Root Flyout
  Referenced<FlyoutGuide<RibbonTool>> root_guide;
  Referenced<Button<NodeTool>> import_flyout_button;
  Referenced<Button<NodeTool>> edit_flyout_button;
  // Import Flyout
  Referenced<FlyoutGuide<NodeTool>> import_guide;
  Referenced<BrowseFile<NodeTool>> file_browser;
  Referenced<ImportSelection<NodeTool>> layer_selection;
  Referenced<ImportSelection<NodeTool>> ID_selection;
  Referenced<ImportSelection<NodeTool>> shape_selection;
  Referenced<ImportSelection<NodeTool>> d1_selection;
  Referenced<ImportSelection<NodeTool>> d2_selection;
  Referenced<ImportSelection<NodeTool>> invert_selection;
  Referenced<ImportSelection<NodeTool>> depth_selection;
  Referenced<Button<NodeTool>> import_execute_button;
  // Edit Flyout
  Referenced<FlyoutGuide<NodeTool>> edit_guide;
  Referenced<Button<NodeTool>> select_button;
  Referenced<Button<NodeTool>> create_button;
  Referenced<Button<NodeTool>> move_button;
  // Create Flyout
  Referenced<FlyoutGuide<NodeTool>> create_guide;
  Referenced<ImportSelection<NodeTool>> create_shape_selection;
  Referenced<InputBox<NodeTool>> create_diameter_input_box;
  Referenced<InputBox<NodeTool>> create_width_input_box;
  Referenced<InputBox<NodeTool>> create_length_input_box;
  Referenced<InputBox<NodeTool>> create_depth_input_box;
  Referenced<InputBox<NodeTool>> create_elev_input_box;
  Referenced<InputBox<NodeTool>> create_ID_input_box;
};

#endif