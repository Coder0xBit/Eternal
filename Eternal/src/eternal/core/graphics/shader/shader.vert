#version 450

layout(location = 0) in vec3 a_Position;

layout(location = 0) out vec3 out_color;

vec3 triangle_colors[4] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 1.0)
);

layout(push_constant) uniform PushConstants
{
	mat4 ViewProjection;
    mat4 Transform;
} u_PushConstants;

void main()
{
    gl_Position = u_PushConstants.ViewProjection
                   * u_PushConstants.Transform
                   * vec4(a_Position, 1.0);

    out_color = triangle_colors[gl_VertexIndex % 4];
}
