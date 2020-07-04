#ifndef TERRAIN
#define TERRAIN

// MARE
#include "Components/RenderPack.hpp"
#include "Mare.hpp"
#include "Materials/PhongMaterial.hpp"
#include "Meshes/GridMesh.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"

using namespace mare;

class Terrain : public RenderPack {
public:
  Terrain(uint32_t n_columns, uint32_t m_rows) {
    mesh = gen_ref<GridMesh>(n_columns, m_rows);
    material = gen_ref<PhongMaterial>();
    gen_system<PacketRenderer>();
    push_packet({mesh, material});
  }

private:
  Referenced<GridMesh> mesh;
  Referenced<PhongMaterial> material;
};

#endif