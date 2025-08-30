#version 450

layout(set = 1, binding = 0) uniform sampler2D texSampler;


layout (location = 0) in vec2 fragUv;

layout (location = 0) out vec4 outColor;

void main() {
      vec2 texCoord = vec2(fragUv.x , 1.0 - fragUv.y);
      outColor = texture(texSampler, texCoord);
}
