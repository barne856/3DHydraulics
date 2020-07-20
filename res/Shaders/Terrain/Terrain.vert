#version 450

in vec4 position;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal_matrix;
uniform vec2 dem_upper_left;
uniform vec2 dem_scale;
uniform int dem_n;
uniform int dem_m;
uniform vec2 image_upper_left;
uniform vec2 image_scale;
uniform int image_n;
uniform int image_m;
uniform float vert_exag = 7.0;

//out vec4 P;
//out vec3 N;
out vec3 amb_color;

layout(std430) buffer model_instances { mat4 models[]; };
layout(std430) buffer elevations { float elev[]; };
layout(std430) buffer reds { float r[]; };
layout(std430) buffer greens { float g[]; };
layout(std430) buffer blues { float b[]; };

vec3 calc_color(vec2 world) {
  int x, y;
  x = int((world.x - image_upper_left.x) / image_scale.x);
  y = int((image_upper_left.y - world.y) / image_scale.y);
  if (x < 0 || x >= image_n || y < 0 || y >= image_m) {
    return vec3(0.0, 0.0, 0.0);
  }
  int image_index = y * image_n + x;
  return vec3(r[image_index] / 255.0, g[image_index] / 255.0,
              b[image_index] / 255.0);
}

vec3 calc_elevation(vec3 world) {
  int x, y;
  x = int((world.x - dem_upper_left.x) / dem_scale.x);
  y = int((dem_upper_left.y - world.y) / dem_scale.y);
  if (x < 0 || x >= dem_n || y < 0 || y >= dem_m) {
    return world;
  }
  int dem_index = y * dem_n + x;
  world.z = elev[dem_index] * vert_exag;
  return world;
}

vec3 calc_normal(vec3 center) {
  vec3 top = vec3(center.x, center.y + dem_scale.y, 0.0);
  top = calc_elevation(top);
  vec3 right = vec3(center.x + dem_scale.x, center.y, 0.0);
  right = calc_elevation(right);
  vec3 bot = vec3(center.x, center.y - dem_scale.y, 0.0);
  bot = calc_elevation(bot);
  vec3 left = vec3(center.x - dem_scale.x, center.y, 0.0);
  left = calc_elevation(left);
  vec3 tl = normalize(cross(top - center, left - center));
  vec3 tr = normalize(cross(right - center, top - center));
  vec3 br = normalize(cross(bot - center, right - center));
  vec3 bl = normalize(cross(left - center, bot - center));
  return (tl + tr + br + bl) / 4.0;
}

void main() {
  vec4 world_pos = model * models[gl_InstanceID] * position;
  world_pos = vec4(calc_elevation(world_pos.xyz), world_pos.w);
  // N = calc_normal(world_pos.xyz);
  // P = view * world_pos;
  amb_color = calc_color(world_pos.xy);
  gl_Position = projection * view * world_pos;
}