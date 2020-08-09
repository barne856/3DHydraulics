#ifndef GRIDHELPER
#define GRIDHELPER

// MARE
#include "Components/RenderPack.hpp"
#include "Materials/BasicColorMaterial.hpp"
#include "Meshes.hpp"
#include "Systems/Rendering/PacketRenderer.hpp"


using namespace mare;

class GridHelperMesh : public SimpleMesh {
public:
  GridHelperMesh(uint32_t n, uint32_t m) {
    std::vector<float> verts;
    set_draw_method(DrawMethod::LINES);
    // draw vertical lines
    float dx = 1.0f / static_cast<float>(n);
    for (uint32_t i = 0; i <= n; i++) {
      verts.push_back(static_cast<float>(i) * dx - 0.5f);
      verts.push_back(0.5f);
      verts.push_back(static_cast<float>(i) * dx - 0.5f);
      verts.push_back(-0.5f);
    }
    // draw horizontal lines
    float dy = 1.0f / static_cast<float>(m);
    for (uint32_t i = 0; i <= m; i++) {
      verts.push_back(-0.5f);
      verts.push_back(-static_cast<float>(i) * dy + 0.5f);
      verts.push_back(0.5f);
      verts.push_back(-static_cast<float>(i) * dy + 0.5f);
    }
    Scoped<Buffer<float>> vertex_buffer =
        Renderer::gen_buffer<float>(&verts[0], verts.size() * sizeof(float));
    vertex_buffer->set_format({{AttributeType::POSITION_2D, "position"}});

    add_geometry_buffer(std::move(vertex_buffer));
  }
};

class GridHelper : public RenderPack {
public:
  GridHelper(uint32_t n, uint32_t m) {
    mesh = gen_ref<GridHelperMesh>(n, m);
    material = gen_ref<BasicColorMaterial>();
    material->set_color(glm::vec4(1.0f));
    push_packet({mesh, material});
    gen_system<PacketRenderer>();
  }
  Referenced<GridHelperMesh> mesh;
  Referenced<BasicColorMaterial> material;
};

#endif