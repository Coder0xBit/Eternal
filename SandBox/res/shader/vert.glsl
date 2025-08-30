#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec2 fragUv;

layout(set = 0 , binding = 0) uniform Uniform {
  mat4 projection;
  mat4 view;
  mat4 model;
} uniformBuffer;

const vec3 LIGHT_POS = vec3(0.0, 0.0, -7.0);
const float AMBIENT = 0.2;

void main() {
  fragUv = uv;
  gl_Position = uniformBuffer.projection * uniformBuffer.view * uniformBuffer.model * vec4(position, 1.0);
}
