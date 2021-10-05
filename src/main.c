#include "platform.h"
#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>

/*
 * TODO LIST:
 * relative file paths
 * frame rate limiting main loop
 * FIGURE THIS CAMERA STUFF OUT
 */

struct Image {
    void *pixels;
    int width;
    int height;

    int bytesPerPixel;
    int pitch;
};

struct Image salamander_loadImage(char *path) {
    struct Image image = { 0 };

    //TODO: do we even want this?
#ifndef SALAMANDER_SCREEN_SPACE
    stbi_set_flip_vertically_on_load(true);
#endif

    image.pixels = stbi_load(path, &image.width, &image.height, &image.bytesPerPixel, 0);
    image.pitch = image.width * image.bytesPerPixel;

    return image;
}

struct Texture salamander_createTextureFromImage(struct Image image) {
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

struct Texture salamander_loadTexture(char *path) {
    struct Image image = salamander_loadImage(path);
    struct Texture texture = salamander_createTextureFromImage(image);

    free(image.pixels);

    return texture;
}

struct Camera {
    vec2 position;

    float rotation;
    float zoom;

    mat4 projectionMatrix;
    mat4 viewMatrix;
};

void salamander_setProjection(struct Camera *camera, float left, float right, float top, float bottom) {
    glm_ortho(left, right, bottom, top, -1.0f, 1.0f, camera->projectionMatrix);
}

//TODO: we don't really need to pass a pointer
void salamander_setViewProjection(struct Shader shader, struct Camera *camera) {
    glm_mat4_identity(camera->viewMatrix);
    glm_translate(camera->viewMatrix, (vec3){ camera->position[0], camera->position[1], 0.0f });

    mat4 inverseView;
    glm_mat4_inv(camera->viewMatrix, inverseView);

    glm_rotate(inverseView, DEGREES_TO_RADIANS(camera->rotation), (vec3){ 0.0f, 0.0f, 1.0f });
    glm_scale(inverseView, (vec3){ camera->zoom, camera->zoom, 0.0f });

    mat4 vp;
    glm_mat4_mul(camera->projectionMatrix, inverseView, vp);

    salamander_setShaderMat4(shader, "u_viewProjection", vp);
}

int main(int argc, char **argv) {
    struct Platform *platform = salamander_createPlatform("SALAMANDER", 1280, 720);
    struct Renderer *renderer = salamander_createRenderer(100);

    struct Shader shader = salamander_loadShader("C:\\dev\\Salamander\\data\\default.glsl");

    struct Camera camera = { 0 };
    camera.zoom = 1.0f;

    salamander_setProjection(&camera, 0.0f, platform->windowWidth, 0.0f, platform->windowHeight);

    vec2 renderScale = { 3.0f, 3.0f };

    vec2 position = { 100.0f, 100.0f };
    float speed = 0.25f;

    struct Texture texture = salamander_loadTexture("C:\\dev\\Salamander\\data\\test.png");
    struct Texture texture2 = salamander_loadTexture("C:\\dev\\Salamander\\data\\test2.png");

    while (!platform->windowClosed) {
        printf("%f\n", camera.zoom);

        if (platform->isKeyDown('D')) camera.position[0] += speed;
        if (platform->isKeyDown('A')) camera.position[0] -= speed;

        if (platform->isKeyDown('S')) camera.position[1] += speed;
        if (platform->isKeyDown('W')) camera.position[1] -= speed;

        if (platform->isKeyDown('Z')) camera.zoom += 0.001f;
        if (platform->isKeyDown('X') && camera.zoom > 0.0f) camera.zoom -= 0.001f;

        if (platform->isKeyDown('R')) camera.rotation += 0.1f;
        if (platform->isKeyDown('E')) camera.rotation -= 0.1f;

        salamander_clearRenderer((vec4){ 0.0f, 0.0f, 0.0f, 1.0f });

        //DEBUG STUFF
        /*glBegin(GL_LINES);

        for (float y = 1; y >= -1.0f; y -= 1.0f / 16.0f) {
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(-1.0f, y);

            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(1.0f, y);
        }

        for (float x = 1; x >= -1.0f; x -= 1.0f / 16.0f) {
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(x, 1.0f);

            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(x, -1.0f);
        }

        glEnd();*/

        salamander_useShader(shader);
        salamander_setViewProjection(shader, &camera);

        salamander_drawTexture(renderer, texture, (vec2){ 0.0f, 0.0f }, renderScale);
        salamander_drawTexture(renderer, texture2, (vec2){ 100.0f, 100.0f }, renderScale);

        salamander_flushRenderer(renderer);
        salamander_useShader(NO_SHADER);

        salamander_updatePlatform(platform);
    }

    return 0;
}