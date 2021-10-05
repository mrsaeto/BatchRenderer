#VERTEX_SHADER
#version 450 core

layout (location = 0) in vec4 a_position;
layout (location = 1) in vec4 a_colour;
layout (location = 2) in vec2 a_textureCoordinates;
layout (location = 3) in float a_textureIndex;

uniform mat4 u_viewProjection;

layout (location = 0) out vec4 o_colour;
layout (location = 1) out vec2 o_textureCoordinates;
layout (location = 2) out flat float o_textureIndex;

void main() {
    o_colour = a_colour;
    o_textureCoordinates = a_textureCoordinates;
    o_textureIndex = a_textureIndex;

    gl_Position = u_viewProjection * a_position;
}

#FRAGMENT_SHADER
#version 450 core

#define MAX_TEXTURE_SLOTS 32

layout (location = 0) in vec4 a_colour;
layout (location = 1) in vec2 a_textureCoordinates;
layout (location = 2) in flat float a_textureIndex;

layout (binding = 0) uniform sampler2D u_textures[MAX_TEXTURE_SLOTS];

void main() {
    vec4 colour = a_colour;

    if (a_textureIndex >= 0 && a_textureIndex < MAX_TEXTURE_SLOTS) {
        colour *= texture(u_textures[int(a_textureIndex)], a_textureCoordinates);
    }

    gl_FragColor = colour;
}