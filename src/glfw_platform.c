#include "platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static Platform g_platform;

static void window_closed_callback(GLFWwindow *window) {
    Platform *platform = glfwGetWindowUserPointer(window);
    platform->window_closed = true;
}

static void mouse_position_callback(GLFWwindow *window, double xpos, double ypos) {
    Platform *platform = glfwGetWindowUserPointer(window);

    platform->mouse_x = (float)xpos;
    platform->mouse_y = (float)ypos;
}

Platform *create_platform(char *title, int width, int height) {
    Platform *platform = &g_platform;

    if (glfwInit()) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
        glfwWindowHint(GLFW_RESIZABLE, false);

        platform->native_window = glfwCreateWindow(1280, 720, "SALAMANDER", NULL, NULL);
        
        //Set callbacks
        glfwSetWindowUserPointer(platform->native_window, platform);
        glfwSetWindowCloseCallback(platform->native_window, window_closed_callback);
        glfwSetCursorPosCallback(platform->native_window, mouse_position_callback);
        
        glfwMakeContextCurrent(platform->native_window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        platform->window_width = width;
        platform->window_height = height;
    }

    return platform;
}

void update_platform(Platform *platform) {
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

int find_line_in_buffer(Buffer file, char *line) {
    char temp[1024] = { 0 };
    int last_offset = 0;

    while (true) {
        int offset = 0;
        char *current = file.data + last_offset;

        while (current[offset] != '\n' && current[offset] != '\0') {
            offset++;
        }

        memcpy_s(temp, 1024, current, offset);
        if (strncmp(temp, line, strnlen(line, 1024)) == 0) {
            return last_offset;
        }
        memset(temp, 0, 1024);

        if (current[offset] == '\0') break;

        last_offset += ++offset;
    }

    return -1;
}