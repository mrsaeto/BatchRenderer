#include "platform.h"
#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>

struct Image {
    void *pixels;
    int width;
    int height;

    int bytes_per_pixel;
    int pitch;
};

struct Image load_image(char *path) {
    struct Image image = { 0 };

    image.pixels = stbi_load(path, &image.width, &image.height, &image.bytes_per_pixel, 0);
    image.pitch = image.width * image.bytes_per_pixel;

    return image;
}

struct Texture create_texture(struct Image image) {
    struct Texture texture = { 0 };

    texture.width = image.width;
    texture.height = image.height;

    glCreateTextures(GL_TEXTURE_2D, 1, &texture.id);
    glTextureStorage2D(texture.id, 1, GL_RGBA8, texture.width, texture.height);

    glTextureParameteri(texture.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(texture.id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture.id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(texture.id, 0, 0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);

    return texture;
}

struct Texture load_texture(char *path) {
    struct Image image = load_image(path);
    struct Texture texture = create_texture(image);

    free(image.pixels);

    return texture;
}

int main(int argc, char **argv) {
    struct Platform *platform = create_platform("SALAMANDER", 1280, 720);
    struct Renderer *renderer = create_renderer(100);

    struct Shader shader = load_shader("C:\\dev\\Salamander\\data\\default.glsl");
    use_shader(shader);

    set_view_projection_matrix(renderer, 0.0f, platform->window_width, platform->window_height, 0.0f);

    mat4 vp;
    get_view_projection_matrix(renderer, vp);
    set_mat4_uniform(shader, "u_view_projection", vp);

    vec2 render_scale = { 3.0f, 3.0f };

    vec2 position = { 100.0f, 100.0f };
    float speed = 0.25f;

    struct Texture texture = load_texture("C:\\dev\\Salamander\\data\\test.png");
    struct Texture texture2 = load_texture("C:\\dev\\Salamander\\data\\test2.png");

    while (!platform->window_closed) {
        if (platform->is_key_down('D')) position[0] += speed;
        if (platform->is_key_down('A')) position[0] -= speed;

        if (platform->is_key_down('S')) position[1] += speed;
        if (platform->is_key_down('W')) position[1] -= speed;

        clear_renderer();

        //add_quad(renderer, position[0], position[1], 64, 64, (vec4){ 0.188f, 0.215f, 0.47f, 1.0f });

        add_texture(renderer, texture, position, render_scale);
        add_texture(renderer, texture2, (vec2){ 200.0f, 100.0f }, render_scale);

        flush_quads(renderer);

        update_platform(platform);
    }

    return 0;
}