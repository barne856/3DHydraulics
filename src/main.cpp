// MARE
#include "GL/GLRenderer.hpp"

// 3DH
#include "Scenes/MainScene.hpp"

using namespace mare;

class HydraulicsProgram : public GLRenderer {
public:
  void init_info() override {
    info.window_title = "3D Hydraulics"; // Window title
    info.window_width = 1280;            // window width in pixels
    info.window_height = 720;            // window height in pixels
    info.window_aspect = 16.0f / 9.0f;   // window aspect ratio
    info.samples = 16;                   // antialiasing samples
    info.fullscreen = false;             // render in fullscreen mode?
    info.vsync = true;   // render in double buffered vsync mode?
    info.debug_mode = 1; // 0000 == off, 0001 == high, 0010 == med, 0100 == low,
                         // 1000 == notification
  }
  void startup() override {
    auto layer = gen_scene<MainScene>();
    load_scene(0);
  }
  void shutdown() override {}
};

int main(int argc, char **argv) { return mare::launch<HydraulicsProgram>(); }