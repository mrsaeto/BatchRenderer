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

// shader code
struct Shader loadShader(char *path);
void useShader(struct Shader shader);

void setShaderMat4(struct Shader shader, char *uniform, mat4 matrix);

// renderer
struct Renderer;
struct Renderer *createRenderer(int maxQuadsPerBatch);

void clearRenderer(vec4 colour);

void drawQuad(struct Renderer *renderer, vec2 position, vec2 size, vec4 colour);
void drawTexture(struct Renderer *renderer, struct Texture texture, vec2 position, vec2 scale);

void flushRenderer(struct Renderer *renderer);

// image stuff
struct Image {
    void *pixels;
    int width;
    int height;
    
    int bytesPerPixel;
    int pitch;
};

struct Image loadImage(char *path);
void freeImage(struct Image *image);

struct Texture loadTexture(char *path);
void freeTexture(struct Texture *texture);

#endif