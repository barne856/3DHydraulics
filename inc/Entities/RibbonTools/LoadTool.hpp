#ifndef LOADTOOL
#define LOADTOOL

#include "Entities/RibbonTool.hpp"
#include "Entities/UI/Button.hpp"
#include "Entities/UI/ColorPicker.hpp"
#include "Entities/UI/Slider.hpp"
#include "Entities/UI/Switch.hpp"
#include "Entities/UI/TextBox.hpp"
#include "Meshes/QuadrangleMesh.hpp"
#include "Renderer.hpp"
using namespace mare;

#include <iostream>

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
      auto junc_button = gen_ref<Button>(layer, bounds, "LOAD JUNCTIONS");
      junc_button->set_on_click_callback(load_junctions);
      auto pipe_button = gen_ref<Button>(layer, bounds, "LOAD PIPES");
      pipe_button->set_on_click_callback(load_pipes);
      push_flyout_element(junc_button,
                          {slot_height, slot_height / 5, slot_height / 10});
      push_flyout_element(pipe_button,
                          {slot_height, slot_height / 5, slot_height / 10});
    }
  }
  static void load_junctions(Layer *layer) {
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog( "shp", NULL, &outPath );
    if ( result == NFD_OKAY )
    {
      // Successful read
      std::cout << outPath << std::endl;
    }
    else if ( result == NFD_CANCEL )
    {
      // User pressed cancel
    }
    else 
    {
      // Error, run NFD_GetError()
    }
  }
  static void load_pipes(Layer *layer) {}
  void on_select() override {}
  void on_deselect() override {}
};

#endif