#include "platform.h"
#include "renderer.h"

struct Camera {
    vec2 position;
    
    float rotation;
    float zoom;
    
    mat4 projectionMatrix;
    mat4 viewMatrix;
};

void setProjection(struct Camera *camera, float left, float right, float top, float bottom) {
    glm_ortho(left, right, bottom, top, -1.0f, 1.0f, camera->projectionMatrix);
}

void setViewProjection(struct Shader shader, struct Camera *camera) {
    glm_mat4_identity(camera->viewMatrix);
    glm_translate(camera->viewMatrix, (vec3){ camera->position[0], camera->position[1], 0.0f });
    
    mat4 inverseView;
    glm_mat4_inv(camera->viewMatrix, inverseView);
    
    glm_rotate(inverseView, DEGREES_TO_RADIANS(camera->rotation), (vec3){ 0.0f, 0.0f, 1.0f });
    glm_scale(inverseView, (vec3){ camera->zoom, camera->zoom, 0.0f });
    
    mat4 vp;
    glm_mat4_mul(camera->projectionMatrix, inverseView, vp);
    
    setShaderMat4(shader, "u_viewProjection", vp);
}

int main(int argc, char **argv) {
    struct Platform *platform = createPlatform("SALAMANDER", 1280, 720);
    struct Renderer *renderer = createRenderer(100);
    
    struct Shader shader = loadShader("C:\\dev\\Salamander\\data\\default.glsl");
    
    struct Camera camera = { 0 };
    camera.zoom = 1.0f;
    
    setProjection(&camera, 0.0f, platform->windowWidth, 0.0f, platform->windowHeight);
    
    vec2 renderScale = { 3.0f, 3.0f };
    
    vec2 position = { 100.0f, 100.0f };
    float speed = 0.25f;
    
    struct Texture texture = loadTexture("C:\\dev\\Salamander\\data\\test.png");
    struct Texture texture2 = loadTexture("C:\\dev\\Salamander\\data\\test2.png");
    
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
        
        clearRenderer((vec4){ 0.0f, 0.0f, 0.0f, 1.0f });
        
        useShader(shader);
        setViewProjection(shader, &camera);
        
        drawTexture(renderer, texture, (vec2){ 0.0f, 0.0f }, renderScale);
        drawTexture(renderer, texture2, (vec2){ 100.0f, 100.0f }, renderScale);
        
        flushRenderer(renderer);
        useShader(NO_SHADER);
        
        updatePlatform(platform);
    }
    
    return 0;
}