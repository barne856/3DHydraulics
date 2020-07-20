#version 450 core

// Output
out vec4 color;
uniform float alpha = 1.0;

// Input from vertex shader
// in vec4 P;
// in vec3 N;
in vec3 amb_color;

// Matrices
// uniform mat4 view;
//
// vec4 material_ambient = vec4(0.1, 0.1, 0.1, 1.0);
// vec4 material_diffuse = vec4(0.5, 0.5, 0.5, 0.0);
// vec4 material_specular = vec4(1.0, 1.0, 1.0, 0.0);
// float material_shininess = 16.0;
//
// vec4 light_ambient = vec4(0.95, 0.95, 0.95, 0.0);
// vec4 light_diffuse = vec4(0.5, 0.5, 0.5, 0.0);
// vec4 light_specular = vec4(1.0, 1.0, 1.0, 0.0);
//
// uniform vec3 light_position = vec3(1.0, 1.0, 1.0);
// uniform float constant_attenuation = 1;
// uniform float linear_attenuation = 1;
// uniform float quadratic_attenuation = 1;
// float strength = 1.0;

void main(void) {
  // material_ambient.r = amb_color.r;
  // material_ambient.g = amb_color.g;
  // material_ambient.b = amb_color.b;
  // vec3 v = normalize(vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0) - P));
  // vec3 l = light_position - vec3(P);
  // float light_distance = length(l);
  // l = l / light_distance;
  // float attenuation =
  //     1.0 / (constant_attenuation + linear_attenuation * light_distance +
  //            quadratic_attenuation * light_distance * light_distance);
  // vec3 eye_direction = vec3(inverse(view) * P);
  // vec3 half_vector = normalize(l + eye_direction);
  //
  // vec3 ambient = material_ambient.rgb * light_ambient.rgb;
  // float diffuse = max(0.0, dot(N, l));
  // float specular = max(0.0, dot(N, half_vector));
  // if (diffuse == 0.0) {
  //   specular = 0.0;
  // } else {
  //   specular = pow(specular, material_shininess) * strength;
  // }
  // vec3 scattered_light = ambient + diffuse * attenuation;
  // vec3 reflected_light = vec3(light_ambient) * specular * attenuation;
  // vec3 rgb = min(scattered_light + reflected_light, vec3(1.0));
  color = vec4(amb_color, alpha);
}