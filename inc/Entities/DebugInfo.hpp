#ifndef DEBUGINFO
#define DEBUGINFO

// MARE
#include "Components/RenderPack.hpp"
#include "Materials/BasicColorMaterial.hpp"
#include "Meshes/CharMesh.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"
using namespace mare;

/**
 * @brief A simple Text Entity used to print Debug info into a Scene or Layer.
 *
 */
class DebugInfo : public RenderPack {
public:
  /**
   * @brief Construct a new DebugInfo Entity
   *
   * @param max_strokes The maximum number of strokes used to construct the
   * CharMesh.
   * @see mare::CharMesh
   */
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