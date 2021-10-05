#ifndef SALAMANDER_RENDERER_H
#define SALAMANDER_RENDERER_H

#include "basic.h"

struct Shader {
    u32 id;
};
#define NO_SHADER (struct Shader) { 0 }

struct Texture {
    int id;
    int width;
    int height;
};

struct Shader salamander_loadShader(char *path);
void salamander_useShader(struct Shader shader);

void salamander_setShaderMat4(struct Shader shader, char *uniform, mat4 matrix);

struct Renderer;

struct Renderer *salamander_createRenderer(int maxQuadsPerBatch);

void salamander_clearRenderer(vec4 colour);

void salamander_drawQuad(struct Renderer *renderer, vec2 position, vec2 size, vec4 colour);
void salamander_drawTexture(struct Renderer *renderer, struct Texture texture, vec2 position, vec2 scale);

void salamander_flushRenderer(struct Renderer *renderer);

#endif