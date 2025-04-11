#version 450

layout (location = 0) in vec3 frag_color;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform Push {
  mat4 transform;
} push;

void main()
{
	out_color = vec4(frag_color, 1.0);
}
