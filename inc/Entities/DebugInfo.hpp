#ifndef DEBUGINFO
#define DEBUGINFO

// MARE
#include "Components/RenderPack.hpp"
#include "Materials/BasicColorMaterial.hpp"
#include "Meshes/CharMesh.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"
using namespace mare;

class DebugInfo : public RenderPack {
public:
  DebugInfo(unsigned int max_strokes) {
    text = gen_ref<CharMesh>("", 0.08f, 0.0f, max_strokes);
    color = gen_ref<BasicColorMaterial>();
    push_packet({text, color});
    gen_system<PacketRenderer>();
  }
  Referenced<CharMesh> text;
  Referenced<BasicColorMaterial> color;
};

#endif