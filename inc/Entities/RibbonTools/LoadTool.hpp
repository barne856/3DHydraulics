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

// 3DH
#include "hyd_util.hpp"

/**
 * @brief A Tool used on the RibbonUI to load data into the Scene.
 *
 */
class LoadTool : public RibbonTool
{
public:
  /**
   * @brief Construct a new LoadTool object
   * @details The LoadTool is to be constructed inside the RibbonUI and pushed
   * onto the tool stack.
   * @param layer The layer which the UIElements will use to draw on.
   * @param slot_height_in_pixels The slot height in pixels, normally the same
   * as the ribbon width in pixels.
   */
  LoadTool(Layer *layer, uint32_t slot_height_in_pixels) : RibbonTool(layer)
  {
    icon = gen_ref<CompositeMesh>();
    auto vertical = gen_ref<QuadrangleMesh>();
    auto horizontal = gen_ref<QuadrangleMesh>();
    vertical->set_scale({0.2f, 0.6f, 1.0f});
    horizontal->set_scale({0.6f, 0.2f, 1.0f});
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(vertical);
    std::dynamic_pointer_cast<CompositeMesh>(icon)->push_mesh(horizontal);
    util::Rect bounds = util::Rect();
    if (layer)
    {
      slot_height =
          slot_height_in_pixels; // total slot height including padding
      // data type selection dropdown box
      data_type_dropdown =
          gen_ref<Dropdown<LoadTool>>(layer, bounds, "DATA TYPE");
      data_type_dropdown->set_selection_options({"MANHOLES", "PIPES"}, 4);
      data_type_dropdown->set_on_select_callback(on_data_type_select, this);
      // Manholes data type
      open_mh_button =
          gen_ref<Button<LoadTool>>(layer, bounds, "OPEN MANHOLE DATA");
      open_mh_button->set_on_click_callback(open_manholes_dataset, this);
      mh_ID_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "ID");
      mh_inv_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "INV");
      mh_dia_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "DIA");
      mh_depth_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "DEPTH");
      import_mh_button =
          gen_ref<Button<LoadTool>>(layer, bounds, "IMPORT MANHOLES");
      import_mh_button->set_on_click_callback(import_manholes, this);
      // Pipes data type
      open_pipe_button =
          gen_ref<Button<LoadTool>>(layer, bounds, "OPEN PIPE DATA");
      open_pipe_button->set_on_click_callback(open_pipe_dataset, this);
      pipe_upID_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "UPID");
      pipe_dnID_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "DNID");
      pipe_updrop_dropdown =
          gen_ref<Dropdown<LoadTool>>(layer, bounds, "UPDROP");
      pipe_dndrop_dropdown =
          gen_ref<Dropdown<LoadTool>>(layer, bounds, "DNDROP");
      pipe_dia_dropdown = gen_ref<Dropdown<LoadTool>>(layer, bounds, "DIA");
      import_pipe_button =
          gen_ref<Button<LoadTool>>(layer, bounds, "IMPORT PIPES");
      import_pipe_button->set_on_click_callback(import_pipes, this);
      // push data type dropdown
      push_flyout_element(data_type_dropdown,
                          {slot_height, slot_height / 5, slot_height / 10});
    }
  }
  static void on_data_type_select(LoadTool *tool)
  {
    int slot_height = tool->slot_height;
    glm::ivec3 slot_vec = {slot_height, slot_height / 5, slot_height / 10};
    tool->clear_flyout_elements();
    tool->push_flyout_element(tool->data_type_dropdown, slot_vec);
    if (tool->data_type_dropdown->get_value() == "MANHOLES")
    {
      tool->push_flyout_element(tool->open_mh_button, slot_vec);
      tool->push_flyout_element(tool->mh_ID_dropdown, slot_vec);
      tool->push_flyout_element(tool->mh_inv_dropdown, slot_vec);
      tool->push_flyout_element(tool->mh_dia_dropdown, slot_vec);
      tool->push_flyout_element(tool->mh_depth_dropdown, slot_vec);
      tool->push_flyout_element(tool->import_mh_button, slot_vec);
    }
    if (tool->data_type_dropdown->get_value() == "PIPES")
    {
      tool->push_flyout_element(tool->open_pipe_button, slot_vec);
      tool->push_flyout_element(tool->pipe_upID_dropdown, slot_vec);
      tool->push_flyout_element(tool->pipe_dnID_dropdown, slot_vec);
      tool->push_flyout_element(tool->pipe_updrop_dropdown, slot_vec);
      tool->push_flyout_element(tool->pipe_dndrop_dropdown, slot_vec);
      tool->push_flyout_element(tool->pipe_dia_dropdown, slot_vec);
      tool->push_flyout_element(tool->import_pipe_button, slot_vec);
    }
    tool->rescale(tool->current_ribbon_width);
  }
  static void open_manholes_dataset(LoadTool *tool)
  {
    tool->manhole_dataset = LoadTool::open_shapefile();
    auto field_names = LoadTool::read_shapefile_fields(tool->manhole_dataset);
    tool->mh_ID_dropdown->set_selection_options(field_names, 4);
    tool->mh_inv_dropdown->set_selection_options(field_names, 4);
    tool->mh_dia_dropdown->set_selection_options(field_names, 4);
    tool->mh_depth_dropdown->set_selection_options(field_names, 4);
  }

  static void import_manholes(LoadTool *tool)
  {
    if (tool->manhole_dataset)
    {
      HydraulicNetwork::LoadedNetwork->import_manholes(
          tool->manhole_dataset, tool->mh_ID_dropdown->get_value(),
          tool->mh_inv_dropdown->get_value(),
          tool->mh_dia_dropdown->get_value(),
          tool->mh_depth_dropdown->get_value());
    }
  }
  static void open_pipe_dataset(LoadTool *tool)
  {
    tool->pipe_dataset = LoadTool::open_shapefile();
    auto field_names = LoadTool::read_shapefile_fields(tool->pipe_dataset);
    tool->pipe_upID_dropdown->set_selection_options(field_names, 4);
    tool->pipe_dnID_dropdown->set_selection_options(field_names, 4);
    tool->pipe_updrop_dropdown->set_selection_options(field_names, 4);
    tool->pipe_dndrop_dropdown->set_selection_options(field_names, 4);
    tool->pipe_dia_dropdown->set_selection_options(field_names, 4);
  }
  static void import_pipes(LoadTool *tool)
  {
    if (tool->pipe_dataset)
    {
      HydraulicNetwork::LoadedNetwork->import_pipes(
          tool->pipe_dataset, tool->pipe_upID_dropdown->get_value(),
          tool->pipe_dnID_dropdown->get_value(),
          tool->pipe_updrop_dropdown->get_value(),
          tool->pipe_dndrop_dropdown->get_value(),
          tool->pipe_dia_dropdown->get_value());
    }
  }
  void on_select() override {}
  void on_deselect() override
  {
    if (manhole_dataset)
    {
      GDALClose(manhole_dataset);
      manhole_dataset = nullptr;
      mh_ID_dropdown->set_selection_options({}, 4);
      mh_inv_dropdown->set_selection_options({}, 4);
      mh_dia_dropdown->set_selection_options({}, 4);
      mh_depth_dropdown->set_selection_options({}, 4);
    }
    if (pipe_dataset)
    {
      GDALClose(pipe_dataset);
      manhole_dataset = nullptr;
      pipe_upID_dropdown->set_selection_options({}, 4);
      pipe_dnID_dropdown->set_selection_options({}, 4);
      pipe_updrop_dropdown->set_selection_options({}, 4);
      pipe_dndrop_dropdown->set_selection_options({}, 4);
      pipe_dia_dropdown->set_selection_options({}, 4);
    }
  }
  static GDALDataset *open_shapefile()
  {
    if (auto shapefile_path = open_file("shp"))
    { // Successful read
      GDALDataset *shapefile = static_cast<GDALDataset *>(
          GDALOpenEx(shapefile_path, GDAL_OF_VECTOR, NULL, NULL, NULL));
      if (shapefile == NULL)
      {
        std::cerr << "Error: Could not open file." << std::endl;
        return nullptr;
      }
      else
      {
        std::cout << "Successfully read file!" << std::endl;
        return shapefile;
      }
    }
    return nullptr;
  }
  static std::vector<std::string>
  read_shapefile_fields(GDALDataset *shapefile)
  {
    if (shapefile)
    {
      OGRLayer *layer;
      layer = shapefile->GetLayer(0);
      layer->ResetReading();
      std::vector<std::string> field_names{};
      int field_count = layer->GetLayerDefn()->GetFieldCount();
      std::cout << std::to_string(field_count) << " Fields in the file."
                << std::endl;
      for (int i = 0; i < field_count; i++)
      {
        field_names.push_back(
            layer->GetLayerDefn()->GetFieldDefn(i)->GetNameRef());
      }
      return field_names;
    }
    return {};
  }

  int slot_height;
  Referenced<Dropdown<LoadTool>> data_type_dropdown;
  GDALDataset *manhole_dataset = nullptr;
  Referenced<Button<LoadTool>> open_mh_button;
  Referenced<Dropdown<LoadTool>> mh_ID_dropdown;
  Referenced<Dropdown<LoadTool>> mh_inv_dropdown;
  Referenced<Dropdown<LoadTool>> mh_dia_dropdown;
  Referenced<Dropdown<LoadTool>> mh_depth_dropdown;
  Referenced<Button<LoadTool>> import_mh_button;
  GDALDataset *pipe_dataset = nullptr;
  Referenced<Button<LoadTool>> open_pipe_button;
  Referenced<Dropdown<LoadTool>> pipe_upID_dropdown;
  Referenced<Dropdown<LoadTool>> pipe_dnID_dropdown;
  Referenced<Dropdown<LoadTool>> pipe_updrop_dropdown;
  Referenced<Dropdown<LoadTool>> pipe_dndrop_dropdown;
  Referenced<Dropdown<LoadTool>> pipe_dia_dropdown;
  Referenced<Button<LoadTool>> import_pipe_button;
};

#endif