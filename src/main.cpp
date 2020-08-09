// MARE
#include "GL/GLRenderer.hpp"

// 3DH
#include "Scenes/MainScene.hpp"

// GDAL
#include <ogrsf_frmts.h>

using namespace mare;

class HydraulicsProgram : public GLRenderer {
public:
  void init_info() override {
    info.window_title = "3D Hydraulics"; // Window title
    info.window_width = 1920;            // window width in pixels
    info.window_height = 1080;            // window height in pixels
    info.window_aspect = 16.0f / 9.0f;   // window aspect ratio
    info.samples = 16;                    // antialiasing samples
    info.fullscreen = false;             // render in fullscreen mode?
    info.vsync = false;  // render in double buffered vsync mode?
    info.debug_mode = 1; // 0000 == off, 0001 == high, 0010 == med, 0100 == low,
                         // 1000 == notification
  }
  void startup() override {
    GDALAllRegister();
    Renderer::enable_primative_restart(true, UINT32_MAX);
    auto layer = gen_scene<MainScene>();
    load_scene(0);
  }
  void shutdown() override {}
};

int main(int argc, char **argv) { return mare::launch<HydraulicsProgram>(); }