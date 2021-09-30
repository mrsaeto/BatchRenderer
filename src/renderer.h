#ifndef SALAMANDER_RENDERER_H
#define SALAMANDER_RENDERER_H

#include "basic.h"

typedef struct {
    u32 id;
} Shader;

Shader load_shader(char *path);
void use_shader(Shader shader);

void set_mat4_uniform(Shader shader, char *uniform, mat4 m);

typedef struct Renderer Renderer;

Renderer *create_renderer(int max_quads_per_batch);
void set_view_projection_matrix(Renderer *renderer, float left, float right, float bottom, float top);
void get_view_projection_matrix(Renderer *renderer, mat4 m);

void clear_renderer();

void add_quad(Renderer *renderer, float x, float y, float width, float height);
void flush_quads(Renderer *renderer);

#endif