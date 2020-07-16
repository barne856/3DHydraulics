#ifndef MATH_3DH
#define MATH_3DH

// Standard Library
#include <vector>

// External Libraries
#include "glm.hpp"

namespace math_3dh {
/**
 * @brief A multiquadric radias basis function used as a parameter to
 * rbf_interp.
 *
 * @param r The radius parameter
 * @return The result of the radial basis function
 * @see math_3dh::rbf_interp()
 */
float multiquadric_rbf(float r);
/**
 * @brief A radial basis function interpolator for scatted 3D points.
 * @details The result is the interpolated elevation of the given \p point
 * through a surface passing through the \p data/
 * @param point The 2D horizontal coordinates to interpolate.
 * @param data The 3D points used to interpolate the surface.
 * @param rbf The radial basis function used to interpolate the \p data.
 * @return The elevation of the \p point interpolated by the \p data.
 */
float rbf_interp(glm::vec2 point, std::vector<glm::vec3> data,
                 float (*rbf)(float));

} // namespace math_3dh

#endif