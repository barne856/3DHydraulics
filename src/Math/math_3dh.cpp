#include "Math/math_3dh.hpp"
#include <Eigen/Dense>
using Eigen::MatrixXf;
using Eigen::VectorXf;

namespace math_3dh {
float multiquadric_rbf(float r) {
  float R = 1.0f; // smoothing parameter
  float e = 3.0f; // shape parameter
  return -sqrtf(R + powf(e * r, 2));
}
float rbf_interp(glm::vec2 point, std::vector<glm::vec3> data,
                 float (*rbf)(float)) {
  // Construct A
  size_t n = data.size();
  MatrixXf A(n, n);
  for (size_t j = 0; j < n; j++) {
    for (size_t i = 0; i < n; i++) {
      if (i == j) {
        A(i, j) = 0.0f;
      } else {
        A(i, j) = rbf(glm::length(glm::vec2(data[i]) - glm::vec2(data[j])));
      }
    }
  }
  // Construct z
  VectorXf z(n);
  for (size_t i = 0; i < n; i++) {
    z(i) = data[i].z;
  }
  // Construct p
  VectorXf p(n);
  for (size_t i = 0; i < n; i++) {
    p(i) = rbf(glm::length(point - glm::vec2(data[i])));
  }
  // elevation(point) = p*(A^(-1)z) = p*w
  VectorXf w = A.partialPivLu().solve(z);
  return p.dot(w);
}
} // namespace math_3dh
