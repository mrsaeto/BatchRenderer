#VERTEX_SHADER
#version 450 core

layout (location = 0) in vec4 a_position;
layout (location = 1) in vec4 a_colour;
layout (location = 2) in vec2 a_texture_coordinates;
layout (location = 3) in float a_texture_index;

uniform mat4 u_view_projection;

layout (location = 0) out vec4 o_colour;
layout (location = 1) out vec2 o_texture_coordinates;
layout (location = 2) out flat float o_texture_index;

void main() {
    o_colour = a_colour;
    o_texture_coordinates = a_texture_coordinates;
    o_texture_index = a_texture_index;

    gl_Position = u_view_projection * a_position;
}

#FRAGMENT_SHADER
#version 450 core

layout (location = 0) in vec4 a_colour;
layout (location = 1) in vec2 a_texture_coordinates;
layout (location = 2) in flat float a_texture_index;

layout (binding = 0) uniform sampler2D u_textures[32];

void main() {
    vec4 colour = a_colour;

    //colour *= texture(u_textures[a_texture_index], a_texture_coordinates);

    switch (int(a_texture_index)) {
        case 0: colour *= texture(u_textures[0], a_texture_coordinates); break;
        case 1: colour *= texture(u_textures[1], a_texture_coordinates); break;
        case 2: colour *= texture(u_textures[2], a_texture_coordinates); break;
        case 3: colour *= texture(u_textures[3], a_texture_coordinates); break;
        case 4: colour *= texture(u_textures[4], a_texture_coordinates); break;
        case 5: colour *= texture(u_textures[5], a_texture_coordinates); break;
        case 6: colour *= texture(u_textures[6], a_texture_coordinates); break;
        case 7: colour *= texture(u_textures[7], a_texture_coordinates); break;
        case 8: colour *= texture(u_textures[8], a_texture_coordinates); break;
        case 9: colour *= texture(u_textures[9], a_texture_coordinates); break;
        case 10: colour *= texture(u_textures[10], a_texture_coordinates); break;
        case 11: colour *= texture(u_textures[11], a_texture_coordinates); break;
        case 12: colour *= texture(u_textures[12], a_texture_coordinates); break;
        case 13: colour *= texture(u_textures[13], a_texture_coordinates); break;
        case 14: colour *= texture(u_textures[14], a_texture_coordinates); break;
        case 15: colour *= texture(u_textures[15], a_texture_coordinates); break;
        case 16: colour *= texture(u_textures[16], a_texture_coordinates); break;
        case 17: colour *= texture(u_textures[17], a_texture_coordinates); break;
        case 18: colour *= texture(u_textures[18], a_texture_coordinates); break;
        case 19: colour *= texture(u_textures[19], a_texture_coordinates); break;
        case 20: colour *= texture(u_textures[20], a_texture_coordinates); break;
        case 21: colour *= texture(u_textures[21], a_texture_coordinates); break;
        case 22: colour *= texture(u_textures[22], a_texture_coordinates); break;
        case 23: colour *= texture(u_textures[23], a_texture_coordinates); break;
        case 24: colour *= texture(u_textures[24], a_texture_coordinates); break;
        case 25: colour *= texture(u_textures[25], a_texture_coordinates); break;
        case 26: colour *= texture(u_textures[26], a_texture_coordinates); break;
        case 27: colour *= texture(u_textures[27], a_texture_coordinates); break;
        case 28: colour *= texture(u_textures[28], a_texture_coordinates); break;
        case 29: colour *= texture(u_textures[29], a_texture_coordinates); break;
        case 30: colour *= texture(u_textures[30], a_texture_coordinates); break;
        case 31: colour *= texture(u_textures[31], a_texture_coordinates); break;
    }

    gl_FragColor = colour;
}