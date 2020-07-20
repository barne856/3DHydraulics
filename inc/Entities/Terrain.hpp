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
  Terrain(RasterDataset *dem, RasterDataset *image, uint32_t k, uint32_t L,
          glm::dvec3 offset = glm::dvec3(0.0, 0.0, 0.0))
      : L(L), offset(offset) {
    glm::dvec2 dem_upper_left = dem->get_top_left_coord();
    dem_upper_left_world_space = {
        static_cast<float>(dem_upper_left.x - offset.x),
        static_cast<float>(dem_upper_left.y - offset.y)};
    dem_pixel_scale = dem->get_pixel_scale();
    dem_grid_offset = {fmodf(dem_upper_left_world_space.x, dem_pixel_scale.x),
                       fmodf(dem_upper_left_world_space.y, dem_pixel_scale.y)};
    dem_pixels = {dem->get_cols(), dem->get_rows()};

    glm::dvec2 image_upper_left = image->get_top_left_coord();
    image_upper_left_world_space = {
        static_cast<float>(image_upper_left.x - offset.x),
        static_cast<float>(image_upper_left.y - offset.y)};
    image_pixel_scale = image->get_pixel_scale();
    image_pixels = {image->get_cols(), image->get_rows()};

    n = pow(2, k) - 1; // clipmap size
    m = (n + 1) / 4;   // footprint size
    for (int i = 0; i < L; i++) {
      cached_snapped_centers.push_back({0.0f, 0.0f});
    }
    interior_block = gen_ref<GridMesh>(n, n);
    block = gen_ref<GridMesh>(m, m);
    ring_fix_up = gen_ref<GridMesh>(3, m);
    interior_trim = gen_ref<GridMesh>(2 * m + 1, 2);
    material = gen_ref<TerrainMaterial>();
    material->set_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    block_instances = gen_ref<InstancedMesh>(12 * (L - 1));
    block_instances->set_mesh(block);
    for (int i = 0; i < 12 * (L - 1); i++) {
      block_instances->push_instance(glm::mat4(1.0f));
    }
    ring_fix_up_instances = gen_ref<InstancedMesh>(4 * (L - 1));
    ring_fix_up_instances->set_mesh(ring_fix_up);
    interior_trim_instances = gen_ref<InstancedMesh>(2 * (L - 1));
    interior_trim_instances->set_mesh(interior_trim);
    interior_block_instance = gen_ref<InstancedMesh>(1);
    interior_block_instance->set_mesh(interior_block);
    interior_block_instance->push_instance(glm::mat4(1.0f));
    init_elevations(dem);
    init_image(image);

    gen_system<TerrainRenderer>();
  }
  void update_footprints(Camera *camera) {
    auto controls = camera->get_system<OrbitControls>();
    float distance_to_center = controls->distance_to_center;
    glm::vec3 dir = camera->get_forward_vector();
    glm::vec3 pos = camera->get_position();
    glm::vec2 world_center = glm::vec2(pos + dir * distance_to_center);
    for (uint32_t l = 0; l < L; l++) {
      glm::vec2 level_scale = {
          static_cast<float>(pow(2, L - l - 1) * dem_pixel_scale.x),
          static_cast<float>(pow(2, L - l - 1) * dem_pixel_scale.y)};
      glm::vec2 level_width = level_scale * static_cast<float>(n - 1);
      glm::vec2 block_width = level_scale * static_cast<float>(m - 1);
      glm::vec2 center_offset{fmodf(world_center.x, 2.0f * level_scale.x),
                              fmodf(world_center.y, 2.0f * level_scale.y)};
      if (center_offset.x < 0.0) {
        center_offset.x += 2.0f * level_scale.x;
      }
      if (center_offset.y < 0.0) {
        center_offset.y += 2.0f * level_scale.y;
      }
      glm::vec2 level_grid_center =
          world_center - center_offset + level_scale + dem_grid_offset;

      if (l == L - 1 || distance_to_center > 2.5f * level_width.x) {
        glm::mat4 scale =
            glm::scale(glm::mat4(1.0f), {level_width.x, level_width.y, 1.0f});
        glm::mat4 translation = glm::translate(
            glm::mat4(1.0f), {level_grid_center.x, level_grid_center.y, 0.0f});
        (*interior_block_instance)[0] = (translation * scale);
        block_instances->set_instance_render_count(12 * l);
        break;
      }

      if (level_grid_center == cached_snapped_centers[l]) {
        continue;
      } else {
        cached_snapped_centers[l] = level_grid_center;
      }

      glm::vec2 level_offset[12] = {
          {-1.0f, 1.0f},  {-1.0f, 1.0f},  {1.0f, 1.0f},   {1.0f, 1.0f},
          {-1.0f, 1.0f},  {1.0f, 1.0f},   {-1.0f, -1.0f}, {1.0f, -1.0f},
          {-1.0f, -1.0f}, {-1.0f, -1.0f}, {1.0f, -1.0f},  {1.0f, -1.0f}};
      glm::vec2 block_offset[12] = {
          {1.0f, -1.0f}, {3.0f, -1.0f},  {-3.0f, -1.0f}, {-1.0f, -1.0f},
          {1.0f, -3.0f}, {-1.0f, -3.0f}, {1.0f, 3.0f},   {-1.0f, 3.0f},
          {1.0f, 1.0f},  {3.0f, 1.0f},   {-3.0f, 1.0f},  {-1.0f, 1.0f}};
      glm::vec3 block_scale = {block_width.x, block_width.y, 1.0f};
      for (int i = 0; i < 12; i++) {
        glm::vec3 block_center = glm::vec3(
            glm::vec2(level_grid_center + level_offset[i] * level_width / 2.0f +
                      block_offset[i] * block_width / 2.0f),
            0.0f);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), block_scale);
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), block_center);
        (*block_instances)[12 * l + i] = (translation * scale);
      }
    }
  }
  /**
   * @brief Convert a world space coordinate to a column and row of the DEM.
   *
   * @param world The world space coordinate
   * @return The column and row of the DEM.
   */
  glm::ivec2 world_to_dem(glm::vec2 world) {
    int x, y;
    x = (world.x - dem_upper_left_world_space.x) / dem_pixel_scale.x;
    y = (dem_upper_left_world_space.y - world.y) / dem_pixel_scale.y;
    return {x, y};
  }
  void init_elevations(RasterDataset *dem) {
    auto elevations =
        dem->read_floats(1, 0, dem_pixels.x - 1, 0, dem_pixels.y - 1,
                         dem_pixels.x, dem_pixels.y, offset.z);
    elev_buffer = Renderer::gen_buffer<float>(
        &elevations[0], sizeof(float) * dem_pixels.x * dem_pixels.y,
        BufferType::READ_WRITE);
  }
  void init_image(RasterDataset *image) {
    auto reds =
        image->read_floats(1, 0, image_pixels.x - 1, 0, image_pixels.y - 1,
                           image_pixels.x, image_pixels.y);
    auto greens =
        image->read_floats(2, 0, image_pixels.x - 1, 0, image_pixels.y - 1,
                           image_pixels.x, image_pixels.y);
    auto blues =
        image->read_floats(3, 0, image_pixels.x - 1, 0, image_pixels.y - 1,
                           image_pixels.x, image_pixels.y);
    red_buffer = Renderer::gen_buffer<float>(
        &reds[0], sizeof(float) * image_pixels.x * image_pixels.y,
        BufferType::READ_WRITE);
    green_buffer = Renderer::gen_buffer<float>(
        &greens[0], sizeof(float) * image_pixels.x * image_pixels.y,
        BufferType::READ_WRITE);
    blue_buffer = Renderer::gen_buffer<float>(
        &blues[0], sizeof(float) * image_pixels.x * image_pixels.y,
        BufferType::READ_WRITE);
  }
  void render(Camera *camera) {
    while (true) {
      GLenum result =
          glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT,
                           32'000'000); // 32ms timeout, single frame at 30 fps
      if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
        glDeleteSync(fence);
        break;
      } else if (result == GL_TIMEOUT_EXPIRED) {
        std::cerr << "Sync timeout expired, slow performance." << std::endl;
        continue;
      } else if (result == GL_WAIT_FAILED) {
        std::cerr << "Non-valid sync object!" << std::endl;
        break;
      }
    }
    update_footprints(camera);
    material->bind();
    material->upload_storage("elevations", elev_buffer.get());
    material->upload_storage("reds", red_buffer.get());
    material->upload_storage("greens", green_buffer.get());
    material->upload_storage("blues", blue_buffer.get());
    material->upload_vec2("dem_upper_left", dem_upper_left_world_space);
    material->upload_vec2("dem_scale", dem_pixel_scale);
    material->upload_int("dem_n", dem_pixels.x);
    material->upload_int("dem_m", dem_pixels.y);
    material->upload_vec2("image_upper_left", image_upper_left_world_space);
    material->upload_vec2("image_scale", image_pixel_scale);
    material->upload_int("image_n", image_pixels.x);
    material->upload_int("image_m", image_pixels.y);
    material->upload_float("vert_exag", vert_exag);
    material->upload_float("alpha", alpha);
    block_instances->render(camera, material.get());
    ring_fix_up_instances->render(camera, material.get());
    interior_trim_instances->render(camera, material.get());
    material->bind();
    material->upload_storage("elevations", elev_buffer.get());
    material->upload_storage("reds", red_buffer.get());
    material->upload_storage("greens", green_buffer.get());
    material->upload_storage("blues", blue_buffer.get());
    material->upload_vec2("dem_upper_left", dem_upper_left_world_space);
    material->upload_vec2("dem_scale", dem_pixel_scale);
    material->upload_int("dem_n", dem_pixels.x);
    material->upload_int("dem_m", dem_pixels.y);
    material->upload_vec2("image_upper_left", image_upper_left_world_space);
    material->upload_vec2("image_scale", image_pixel_scale);
    material->upload_int("image_n", image_pixels.x);
    material->upload_int("image_m", image_pixels.y);
    material->upload_float("vert_exag", vert_exag);
    material->upload_float("alpha", alpha);
    interior_block_instance->render(camera, material.get());
    fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  }
  void set_vert_exag(float value) { vert_exag = value; }
  void set_alpha(float value) { alpha = value; }

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