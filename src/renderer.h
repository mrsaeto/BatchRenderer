#ifndef SALAMANDER_RENDERER_H
#define SALAMANDER_RENDERER_H

#include "basic.h"

struct Shader {
    u32 id;
};

struct Texture {
    int id;
    int width;
    int height;
};

struct Shader load_shader(char *path);
void use_shader(struct Shader shader);

void set_mat4_uniform(struct Shader shader, char *uniform, mat4 m);

struct Renderer;

struct Renderer *create_renderer(int max_quads_per_batch);
void set_view_projection_matrix(struct Renderer *renderer, float left, float right, float bottom, float top);
void get_view_projection_matrix(struct Renderer *renderer, mat4 m);

void clear_renderer();

void add_quad(struct Renderer *renderer, float x, float y, float width, float height, vec4 colour);
void add_texture(struct Renderer *renderer, struct Texture texture, vec2 position, vec2 scale);

void flush_quads(struct Renderer *renderer);

#endif