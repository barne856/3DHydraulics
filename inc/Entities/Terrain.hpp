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
  Terrain(std::string filepath, uint32_t k, uint32_t L,
          glm::dvec3 offset = glm::dvec3(0.0, 0.0, 0.0))
      : L(L), offset(offset) {
    dem = gen_ref<RasterDataset>(filepath);
    glm::dvec2 dem_upper_left = dem->get_top_left_coord();
    dem_upper_left_world_space = {
        static_cast<float>(dem_upper_left.x - offset.x),
        static_cast<float>(dem_upper_left.y - offset.y)};
    dem_pixel_scale = dem->get_pixel_scale();
    dem_grid_offset = {fmodf(dem_upper_left_world_space.x, dem_pixel_scale.x),
                       fmodf(dem_upper_left_world_space.y, dem_pixel_scale.y)};
    dem_pixels = {dem->get_cols(), dem->get_rows()};
    n = pow(2, k) - 1; // clipmap size
    m = (n + 1) / 4;   // footprint size
    interior_block = gen_ref<GridMesh>(n, n);
    block = gen_ref<GridMesh>(m, m);
    ring_fix_up = gen_ref<GridMesh>(3, m);
    interior_trim = gen_ref<GridMesh>(2 * m + 1, 2);
    material = gen_ref<TerrainMaterial>();
    material->set_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    block_instances = gen_ref<InstancedMesh>(12 * (L - 1));
    block_instances->set_mesh(block);
    ring_fix_up_instances = gen_ref<InstancedMesh>(4 * (L - 1));
    ring_fix_up_instances->set_mesh(ring_fix_up);
    interior_trim_instances = gen_ref<InstancedMesh>(2 * (L - 1));
    interior_trim_instances->set_mesh(interior_trim);
    interior_block_instance = gen_ref<InstancedMesh>(1);
    interior_block_instance->set_mesh(interior_block);
    init_footprint_instances();

    gen_system<TerrainRenderer>();
  }
  void init_footprint_instances() {
    std::vector<float> elevations =
        dem->read_floats(1, 0, dem_pixels.x - 1, 0, dem_pixels.y - 1,
                         dem_pixels.x, dem_pixels.y, offset.z);
    elev_buffer = Renderer::gen_buffer<float>(
        &elevations[0], sizeof(float) * dem_pixels.x * dem_pixels.y);
    material->bind();
    material->upload_storage("elevations", elev_buffer.get());

    glm::vec2 center = {0.0f, 0.0f};
    for (uint32_t l = 0; l < L; l++) {
      glm::vec2 level_pixel_width = {
          static_cast<float>(pow(2, L - l - 1) * dem_pixel_scale.x),
          static_cast<float>(pow(2, L - l - 1) * dem_pixel_scale.y)};
      glm::vec2 level_width = level_pixel_width * static_cast<float>(n - 1);

      glm::vec2 snapped_center =
          glm::floor(center / (2.0f * level_pixel_width)) * 2.0f *
              level_pixel_width +
          level_pixel_width + dem_grid_offset;
      cached_snapped_centers.push_back(snapped_center);

      if (l == L - 1) {
        glm::mat4 scale =
            glm::scale(glm::mat4(1.0f), {level_width.x, level_width.y, 1.0f});
        glm::mat4 translation = glm::translate(
            glm::mat4(1.0f), {snapped_center.x, snapped_center.y, 0.0f});
        interior_block_instance->push_instance(translation * scale);
      } else {
        glm::vec2 block_width = level_pixel_width * static_cast<float>(m - 1);

        glm::mat4 scale =
            glm::scale(glm::mat4(1.0f), {block_width.x, block_width.y, 1.0f});
        // blocks, top row
        glm::mat4 translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 0.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 0.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 1.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 0.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 1.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 0.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 0.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 0.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        // blocks, second row
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 0.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 1.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 0.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 1.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        // blocks, third row
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 0.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 1.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 0.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 1.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        // blocks, bottom row
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 0.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 0.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 1.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 0.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 1.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 0.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 0.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 0.5 * block_width.y,
             0.0f});
        block_instances->push_instance(translation * scale);
      }
    }
  }
  void gen_footprint_instances(Camera *camera) {
    auto controls = camera->get_system<OrbitControls>();
    float distance_to_center = controls->distance_to_center;
    glm::vec3 dir = camera->get_forward_vector();
    glm::vec3 pos = camera->get_position();
    glm::vec2 center = glm::vec2(pos + dir * distance_to_center);
    // gen_elevation_raster_clipmaps(center);
    for (uint32_t l = 0; l < L; l++) {
      glm::vec2 level_pixel_width = {
          static_cast<float>(pow(2, L - l - 1) * dem_pixel_scale.x),
          static_cast<float>(pow(2, L - l - 1) * dem_pixel_scale.y)};
      glm::vec2 level_width = level_pixel_width * static_cast<float>(n - 1);

      glm::vec2 snapped_center =
          glm::floor(center / (2.0f * level_pixel_width)) * 2.0f *
              level_pixel_width +
          level_pixel_width + dem_grid_offset;
      // if (snapped_center == cached_snapped_centers[l] && distance_to_center
      // <= 2.5f * level_width.x) {
      //  continue;
      //} else {
      //  cached_snapped_centers[l] = snapped_center;
      //}

      if (l == L - 1 || distance_to_center > 2.5f * level_width.x) {
        glm::mat4 scale =
            glm::scale(glm::mat4(1.0f), {level_width.x, level_width.y, 1.0f});
        glm::mat4 translation = glm::translate(
            glm::mat4(1.0f), {snapped_center.x, snapped_center.y, 0.0f});
        (*interior_block_instance)[0] = (translation * scale);
        block_instances->set_instance_render_count(12 * l);
        break;
      } else {
        glm::vec2 block_width = level_pixel_width * static_cast<float>(m - 1);

        glm::mat4 scale =
            glm::scale(glm::mat4(1.0f), {block_width.x, block_width.y, 1.0f});
        // blocks, top row
        glm::mat4 translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 0.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 0.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 0] = (translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 1.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 0.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 1] = (translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 1.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 0.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 2] = (translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 0.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 0.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 3] = (translation * scale);
        // blocks, second row
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 0.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 1.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 4] = (translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 0.5 * block_width.x,
             snapped_center.y + 0.5f * level_width.y - 1.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 5] = (translation * scale);
        // blocks, third row
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 0.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 1.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 6] = (translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 0.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 1.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 7] = (translation * scale);
        // blocks, bottom row
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 0.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 0.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 8] = (translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x - 0.5f * level_width.x + 1.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 0.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 9] = (translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 1.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 0.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 10] = (translation * scale);
        translation = glm::translate(
            glm::mat4(1.0f),
            {snapped_center.x + 0.5f * level_width.x - 0.5 * block_width.x,
             snapped_center.y - 0.5f * level_width.y + 0.5 * block_width.y,
             0.0f});
        (*block_instances)[12 * l + 11] = (translation * scale);
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
  void gen_elevation_raster_clipmaps(glm::vec2 center) {
    glm::vec2 L_pixel_width = {
        static_cast<float>(pow(2, L - 1) * dem_pixel_scale.x),
        static_cast<float>(pow(2, L - 1) * dem_pixel_scale.y)};
    glm::vec2 L_width = L_pixel_width * static_cast<float>(n - 1);
    glm::ivec2 tl = world_to_dem({center.x - L_width.x, center.y + L_width.y});
    glm::ivec2 br = world_to_dem({center.x + L_width.x, center.y - L_width.y});
    std::vector<float> elevations =
        dem->read_floats(1, tl.x, br.x, tl.y, br.y, n, n);
    if (!elev_buffer) {
      elev_buffer = Renderer::gen_buffer<float>(
          &elevations[0], sizeof(float) * n * n, BufferType::READ_WRITE);
    } else {
      elev_buffer->flush(&elevations[0], 0, sizeof(float) * n * n);
    }
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
    gen_footprint_instances(camera);
    material->bind();
    material->upload_storage("elevations", elev_buffer.get());
    material->upload_vec2("dem_upper_left", dem_upper_left_world_space);
    material->upload_vec2("dem_scale", dem_pixel_scale);
    material->upload_int("dem_n", dem_pixels.x);
    block_instances->render(camera, material.get());
    ring_fix_up_instances->render(camera, material.get());
    interior_trim_instances->render(camera, material.get());
    material->bind();
    material->upload_storage("elevations", elev_buffer.get());
    material->upload_vec2("dem_upper_left", dem_upper_left_world_space);
    material->upload_vec2("dem_scale", dem_pixel_scale);
    material->upload_int("dem_n", dem_pixels.x);
    interior_block_instance->render(camera, material.get());
    fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  }

private:
  GLsync fence;
  Referenced<GridMesh> interior_block;
  Referenced<GridMesh> block;
  Referenced<GridMesh> ring_fix_up;
  Referenced<GridMesh> interior_trim;
  Referenced<TerrainMaterial> material;
  Referenced<InstancedMesh> block_instances;
  Referenced<InstancedMesh> ring_fix_up_instances;
  Referenced<InstancedMesh> interior_trim_instances;
  Referenced<InstancedMesh> interior_block_instance;
  Referenced<RasterDataset> dem;
  glm::vec2 dem_upper_left_world_space;
  glm::vec2 dem_pixel_scale;
  glm::vec2 dem_grid_offset;
  glm::ivec2 dem_pixels;
  uint32_t L;
  uint32_t n;
  uint32_t m;
  glm::dvec3 offset;
  std::vector<glm::vec2> cached_snapped_centers{};
  Referenced<Buffer<float>> elev_buffer = nullptr;
};

class TerrainRenderer : public RenderSystem<Terrain> {
public:
  void render(float dt, Camera *camera, Terrain *terrain) override {
    terrain->render(camera);
  }
};

#endif