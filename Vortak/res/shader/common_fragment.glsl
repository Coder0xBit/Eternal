#version 450

layout(location = 0) in vec3 fragInColor;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragInColor, 1.0);
}
