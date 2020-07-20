#ifndef TERRAINMATERIAL
#define TERRAINMATERIAL

// External Libraries
#include "glm.hpp"

// MARE
#include "Shader.hpp"

namespace mare {
/**
 * @brief A Terrain Material
 */
class TerrainMaterial : public virtual Material {
public:
  /**
   * @brief Construct a new Terrain Material
   */
  TerrainMaterial()
      : Material("./res/Shaders/Terrain"), m_color(glm::vec4(1.0f)) {}
  virtual ~TerrainMaterial() {}
  /**
   * @brief uploads the color to the shader when rendered.
   *
   */
  void render() override {}
  /**
   * @brief Set the color of the Material.
   *
   * @param color The color to set.
   */
  inline void set_color(glm::vec4 color) { m_color = color; }

protected:
  glm::vec4 m_color;
};
} // namespace mare

#endif