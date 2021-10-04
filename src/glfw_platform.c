#include "platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static struct Platform g_platform;

static void window_closed_callback(GLFWwindow *window) {
    struct Platform *platform = glfwGetWindowUserPointer(window);
    platform->window_closed = true;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    struct Platform *platform = glfwGetWindowUserPointer(window);

    bool is_down = (action == GLFW_PRESS);
    bool was_down = (action == GLFW_RELEASE) || (action == GLFW_REPEAT);

    if (key >= 0 && key < INPUT_KEY_BUFFER_SIZE) {
        bool is_pressed = is_down && !was_down;
        bool is_released = !is_down && !was_down;

        u8 state = 0;
        state = (state | is_released) << 1;
        state = (state | is_pressed) << 1;
        state |= is_down;

        platform->key_state[key] = state;
    }
}

static void mouse_position_callback(GLFWwindow *window, double xpos, double ypos) {
    struct Platform *platform = glfwGetWindowUserPointer(window);

    platform->mouse_x = (float)xpos;
    platform->mouse_y = (float)ypos;
}

//NOTE: these functions access global data
static bool f_is_key_down(int key) {
    if (key < 0 || key >= INPUT_KEY_BUFFER_SIZE) {
        return false;
    }

    u8 state = g_platform.key_state[key];
    return state & 0b0001;
}

static bool f_is_key_pressed(int key) {
    if (key < 0 || key >= INPUT_KEY_BUFFER_SIZE) {
        return false;
    }

    u8 state = g_platform.key_state[key];
    return state & 0b0010;
}

//TODO: this function does not work
static bool f_is_key_released(int key) {
    if (key < 0 || key >= INPUT_KEY_BUFFER_SIZE) {
        return false;
    }

    u8 state = g_platform.key_state[key];
    return state & 0b0100;
}

struct Platform *create_platform(char *title, int width, int height) {
    struct Platform *platform = &g_platform;

    if (glfwInit()) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
        glfwWindowHint(GLFW_RESIZABLE, false);

        platform->native_window = glfwCreateWindow(width, height, title, NULL, NULL);
        
        //Set callbacks
        glfwSetWindowUserPointer(platform->native_window, platform);
        glfwSetWindowCloseCallback(platform->native_window, window_closed_callback);
        glfwSetKeyCallback(platform->native_window, key_callback);
        glfwSetCursorPosCallback(platform->native_window, mouse_position_callback);
        
        glfwMakeContextCurrent(platform->native_window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        platform->window_width = width;
        platform->window_height = height;

        platform->is_key_down = f_is_key_down;
        platform->is_key_pressed = f_is_key_pressed;
        platform->is_key_released = f_is_key_released;
    }

    return platform;
}

void update_platform(struct Platform *platform) {
    for (int i = 0; i < INPUT_KEY_BUFFER_SIZE; i++) {
        platform->key_state[i] &= 0b0001;
    }

    glfwSwapBuffers(platform->native_window);
    glfwPollEvents();
}

//TODO: these dont belong here
Buffer read_file_into_buffer(char *path) {
    Buffer file = { 0 };

    FILE *handle;
    fopen_s(&handle, path, "r");

    fseek(handle, 0, SEEK_END);
    file.size = ftell(handle);
    fseek(handle, 0, SEEK_SET);

    file.data = malloc(file.size + 1);
    memset(file.data, 0, file.size + 1);

    size_t bytesRead = fread_s(file.data, file.size, sizeof(u8), file.size, handle);
    file.data[bytesRead] = '\0';

    fclose(handle);

    return file;
}

int find_line_in_buffer(Buffer buffer, char *line) {
    const int temp_buffer_size = 1024;

    char temp[1024] = { 0 };
    int last_offset = 0;

    while (true) {
        int offset = 0;
        char *current = buffer.data + last_offset;

        while (current[offset] != '\n' && current[offset] != '\0') {
            offset++;
        }

        memcpy_s(temp, temp_buffer_size, current, offset);
        if (strncmp(temp, line, strnlen(line, temp_buffer_size)) == 0) {
            return last_offset;
        }
        memset(temp, 0, temp_buffer_size);

        if (current[offset] == '\0') break;

        last_offset += ++offset;
    }

    return -1;
}