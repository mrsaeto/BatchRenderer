#include "platform.h"
#include "renderer.h"

int main(int argc, char **argv) {
    Platform *platform = create_platform("SALAMANDER", 1280, 720);
    Renderer *renderer = create_renderer(100);

    Shader shader = load_shader("C:\\dev\\Salamander\\data\\default.shader");
    use_shader(shader);

    set_view_projection_matrix(renderer, 0.0f, platform->window_width, platform->window_height, 0.0f);

    mat4 vp;
    get_view_projection_matrix(renderer, vp);
    set_mat4_uniform(shader, "u_view_projection", vp);

    while (!platform->window_closed) {
        clear_renderer();

        add_quad(renderer, 0, 0, 32, 32);
        add_quad(renderer, 100, 300, 128, 128);

        flush_quads(renderer);

        update_platform(platform);
    }

    return 0;
}