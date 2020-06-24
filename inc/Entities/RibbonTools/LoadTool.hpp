#ifndef LOADTOOL
#define LOADTOOL

#include "Entities/RibbonTool.hpp"
#include "Entities/UI/Button.hpp"
#include "Entities/UI/ColorPicker.hpp"
#include "Entities/UI/Dropdown.hpp"
#include "Entities/UI/Slider.hpp"
#include "Entities/UI/Switch.hpp"
#include "Entities/UI/TextBox.hpp"
#include "Meshes/QuadrangleMesh.hpp"
#include "Renderer.hpp"
using namespace mare;

#include <iostream>

// GDAL
#include <ogrsf_frmts.h>

// EXT
#include "nfd.h"

/**
 * @brief A Tool used on the RibbonUI to load data into the Scene.
 *
 */
class LoadTool : public RibbonTool {
public:
  /**
   * @brief Construct a new LoadTool object
   * @details The LoadTool is to be constructed inside the RibbonUI and pushed
   * onto the tool stack.
   * @param layer The layer which the UIElements will use to draw on.
   * @param slot_height_in_pixels The slot height in pixels, normally the same
   * as the ribbon width in pixels.
   */
  LoadTool(Layer *layer, uint32_t slot_height_in_pixels) : RibbonTool(layer) {
    icon = gen_ref<CompositeMesh>();
    auto vertical = gen_ref<QuadrangleMesh>();
    auto horizontal = gen_ref<QuadrangleMesh>();
    vertical->set_scale({0.2f, 0.6f, 1.0f});
    horizontal->set_scale({0.6f, 0.2f, 1.0f});
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(vertical);
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(horizontal);
    util::Rect bounds = util::Rect();
    if (layer) {
      int slot_height =
          slot_height_in_pixels; // total slot height including padding
      auto open_junc_button =
          gen_ref<Button<LoadTool>>(layer, bounds, "OPEN FILE");
      open_junc_button->set_on_click_callback(open_junctions, this);
      auto import_junc_button =
          gen_ref<Button<LoadTool>>(layer, bounds, "IMPORT JUNCTIONS");
      import_junc_button->set_on_click_callback(import_junctions, this);
      ID_dropdown = gen_ref<Dropdown>(layer, bounds, "ID");
      INV_dropdown = gen_ref<Dropdown>(layer, bounds, "INV");
      DIA_dropdown = gen_ref<Dropdown>(layer, bounds, "DIA");
      DEPTH_dropdown = gen_ref<Dropdown>(layer, bounds, "DEPTH");
      push_flyout_element(open_junc_button,
                          {slot_height, slot_height / 5, slot_height / 10});
      push_flyout_element(ID_dropdown,
                          {slot_height, slot_height / 5, slot_height / 10});
      push_flyout_element(INV_dropdown,
                          {slot_height, slot_height / 5, slot_height / 10});
      push_flyout_element(DIA_dropdown,
                          {slot_height, slot_height / 5, slot_height / 10});
      push_flyout_element(DEPTH_dropdown,
                          {slot_height, slot_height / 5, slot_height / 10});
      push_flyout_element(import_junc_button,
                          {slot_height, slot_height / 5, slot_height / 10});
    }
  }
  static void open_junctions(LoadTool *tool) {
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog("shp", NULL, &outPath);
    if (result == NFD_OKAY) {
      // Successful read
      GDALDataset *poDS;
      poDS = static_cast<GDALDataset *>(
          GDALOpenEx(outPath, GDAL_OF_VECTOR, NULL, NULL, NULL));
      if (poDS == NULL) {
        std::cerr << "Error: Could not open file." << std::endl;
        return;
      } else {
        std::cout << "Successfully read file!" << std::endl;
        OGRLayer *poLayer;
        poLayer = poDS->GetLayer(0);
        OGRFeature *poFeature;
        poLayer->ResetReading();
        std::vector<std::string> field_names{};
        int field_count = poLayer->GetLayerDefn()->GetFieldCount();
        std::cout << std::to_string(field_count) << " Fields in the file."
                  << std::endl;
        for (int i = 0; i < field_count; i++) {
          field_names.push_back(
              poLayer->GetLayerDefn()->GetFieldDefn(i)->GetNameRef());
        }
        GDALClose(poDS);
        tool->ID_dropdown->set_selection_options(field_names, 4);
        tool->INV_dropdown->set_selection_options(field_names, 4);
        tool->DIA_dropdown->set_selection_options(field_names, 4);
        tool->DEPTH_dropdown->set_selection_options(field_names, 4);
      }
    } else if (result == NFD_CANCEL) {
      // User pressed cancel
    } else {
      // Error, run NFD_GetError()
    }
  }
  static void import_junctions(LoadTool* tool)
  {
    
  }
  void on_select() override {}
  void on_deselect() override {}

private:
  Referenced<Dropdown> ID_dropdown;
  Referenced<Dropdown> INV_dropdown;
  Referenced<Dropdown> DIA_dropdown;
  Referenced<Dropdown> DEPTH_dropdown;
};

#endif