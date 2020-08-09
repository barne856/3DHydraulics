#ifndef TERRAIN
#define TERRAIN

// Standard Library
#include <cmath>

// MARE
#include "Mare.hpp"
#include "Meshes/GridMesh.hpp"
#include "Systems.hpp"
#include "Systems/Controls/OrbitControls.hpp"

// 3DH
#include "GDAL/gdal_io.hpp"
#include "Materials/TerrainMaterial.hpp"

using namespace mare;
using namespace gdal_input;

// forward declare rendering system
class TerrainRenderer;

class Terrain : public Entity {
public:
  /**
   * @brief Construct a new Terrain object
   *
   * @param filepath The filepath to a DEM used to construct the terrain.
   * @param k The clipmap power. n = 2^k-1 where n is the number of vertices in
   * both the horizontal and vertical dimensions for each clipmap level. 8 is a
   * good value.
   * @param L The number of levels used for the clipmap.
   */
  Terrain(RasterDataset *dem, RasterDataset *image, uint32_t k, uint32_t L);
  float get_terrain_elevation(glm::vec2 center);
  glm::dvec3 calc_offset(RasterDataset *dem, RasterDataset *image);
  void update_footprints(Camera *camera);
  /**
   * @brief Convert a world space coordinate to a column and row of the DEM.
   *
   * @param world The world space coordinate
   * @return The column and row of the DEM.
   */
  glm::ivec2 world_to_dem(glm::vec2 world);
  void init_elevations(RasterDataset *dem);
  void init_image(RasterDataset *image);
  void render(Camera *camera);
  inline void set_vert_exag(float value) { vert_exag = value; }
  inline float get_vert_exag() { return vert_exag; }
  inline void set_alpha(float value) { alpha = value; }

private:
  float vert_exag = 1.0f;
  float alpha = 1.0f;
  GLsync fence;
  std::vector<glm::vec2> cached_snapped_centers{};
  Referenced<GridMesh> interior_block;
  Referenced<GridMesh> block;
  Referenced<GridMesh> ring_fix_up;
  Referenced<GridMesh> interior_trim;
  Referenced<TerrainMaterial> material;
  Referenced<InstancedMesh> block_instances;
  Referenced<InstancedMesh> ring_fix_up_instances;
  Referenced<InstancedMesh> interior_trim_instances;
  Referenced<InstancedMesh> interior_block_instance;

  glm::vec2 dem_upper_left_world_space;
  glm::vec2 dem_pixel_scale;
  glm::vec2 dem_grid_offset;
  glm::ivec2 dem_pixels;
  float dem_no_data_value;

  glm::vec2 image_upper_left_world_space;
  glm::vec2 image_pixel_scale;
  glm::vec2 image_grid_offset;
  glm::ivec2 image_pixels;

  uint32_t L;
  uint32_t n;
  uint32_t m;
  glm::dvec3 offset;
  Referenced<Buffer<float>> elev_buffer = nullptr;
  Referenced<Buffer<float>> red_buffer = nullptr;
  Referenced<Buffer<float>> green_buffer = nullptr;
  Referenced<Buffer<float>> blue_buffer = nullptr;
};

class TerrainRenderer : public RenderSystem<Terrain> {
public:
  void render(float dt, Camera *camera, Terrain *terrain) override {
    terrain->render(camera);
  }
};

#endif