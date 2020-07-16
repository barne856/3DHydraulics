#version 450

in vec4 position;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 dem_upper_left;
uniform vec2 dem_scale;
uniform int dem_n;
uniform float vert_exag = 10.0;

layout(std430, binding = 0) buffer model_instances { mat4 models[]; };
layout(std430, binding = 0) buffer elevations { float elev[]; };

ivec2 world_to_dem(vec2 world) {
  int x, y;
  x = int((world.x - dem_upper_left.x) / dem_scale.x);
  y = int((dem_upper_left.y - world.y) / dem_scale.y);
  return ivec2(x, y);
}

void main() {
  vec4 world_pos = model * models[gl_InstanceID] * position;
  ivec2 dem_index = world_to_dem(world_pos.xy);
  world_pos.z = elev[dem_index.y * dem_n + dem_index.x]*vert_exag;
  gl_Position = projection * view * world_pos;
}