#VERTEX_SHADER
#version 450 core

layout (location = 0) in vec4 a_position;

uniform mat4 u_view_projection;

void main() {
    gl_Position = u_view_projection * a_position;
}

#FRAGMENT_SHADER
#version 450 core

out vec4 o_frag_colour;

void main() {
    o_frag_colour = vec4(0.19, 0.27, 0.66, 1.0);
}