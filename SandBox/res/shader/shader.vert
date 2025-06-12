#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  mat4 transform; // projection * view * model
  mat4 normalMatrix;
  mat4 modelMatrix;
} push;

const vec3 LIGHT_POS = vec3(0.0, 0.0, -7.0);
const float AMBIENT = 0.2;

void main() {

  vec4 worldPosition = push.modelMatrix * vec4(position, 1.0);
  vec3 lightDirection = normalize(LIGHT_POS - worldPosition.xyz);
  vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

  float lightIntensity = AMBIENT + max(dot(normalWorldSpace, lightDirection), 0);
  float distance = length(LIGHT_POS - worldPosition.xyz);
  lightIntensity /= (distance * distance); 

  fragColor = lightIntensity * color;
  gl_Position = push.transform * vec4(position, 1.0);

}
