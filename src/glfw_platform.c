#include "platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static struct Platform g_platform;

static void windowClosedCallback(GLFWwindow *window) {
    struct Platform *platform = glfwGetWindowUserPointer(window);
    platform->windowClosed = true;
}

static void windowKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    struct Platform *platform = glfwGetWindowUserPointer(window);

    bool isDown = (action == GLFW_PRESS) || (action == GLFW_REPEAT);
    bool wasDown = (action == GLFW_RELEASE) || (action == GLFW_REPEAT);

    if (key >= 0 && key < INPUT_KEY_BUFFER_SIZE) {
        bool isPressed = isDown && !wasDown;
        bool isReleased = !isDown && wasDown;

        u8 state = 0;
        state = (state | isPressed) << 1;
        state = (state | isReleased) << 1;
        state |= isDown;

        platform->keyState[key] = state;
    }
}

static void windowMousePosCallback(GLFWwindow *window, double xpos, double ypos) {
    struct Platform *platform = glfwGetWindowUserPointer(window);

    platform->mouseX = (float)xpos;
    platform->mouseY = (float)ypos;
}

static void windowMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    struct Platform *platform = glfwGetWindowUserPointer(window);

    bool isDown = (action == GLFW_PRESS);
    bool wasDown = (action == GLFW_RELEASE) || (action == GLFW_REPEAT);

    if (button >= 0 && button < INPUT_BUTTON_BUFFER_SIZE) {
        bool isPressed = isDown && !wasDown;
        bool isReleased = !isDown && wasDown;

        u8 state = 0;
        state = (state | isPressed) << 1;
        state = (state | isReleased) << 1;
        state |= isDown;

        platform->buttonState[button] = state;
    }
}

//TODO: do we want these functions to access global data?
static bool fp_isKeyDown(int key) {
    if (key < 0 || key >= INPUT_KEY_BUFFER_SIZE) {
        return false;
    }

    u8 state = g_platform.keyState[key];
    return state & 0b0001;
}

static bool fp_isKeyPressed(int key) {
    if (key < 0 || key >= INPUT_KEY_BUFFER_SIZE) {
        return false;
    }

    u8 state = g_platform.keyState[key];
    return state & 0b0100;
}

static bool fp_isKeyReleased(int key) {
    if (key < 0 || key >= INPUT_KEY_BUFFER_SIZE) {
        return false;
    }

    u8 state = g_platform.keyState[key];
    return state & 0b0010;
}

static bool fp_isMouseButtonDown(int button) {
    if (button < 0 || button >= INPUT_BUTTON_BUFFER_SIZE) {
        return false;
    }

    u8 state = g_platform.buttonState[button];
    return state & 0b0001;
}

static bool fp_isMouseButtonPressed(int button) {
    if (button < 0 || button >= INPUT_BUTTON_BUFFER_SIZE) {
        return false;
    }

    u8 state = g_platform.buttonState[button];
    return state & 0b0100;
}

static bool fp_isMouseButtonReleased(int button) {
    if (button < 0 || button >= INPUT_BUTTON_BUFFER_SIZE) {
        return false;
    }

    u8 state = g_platform.buttonState[button];
    return state & 0b0010;
}

struct Platform *salamander_createPlatform(char *title, int width, int height) {
    struct Platform *platform = &g_platform;

    if (glfwInit()) {
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
        glfwWindowHint(GLFW_RESIZABLE, false);

        platform->nativeWindow = glfwCreateWindow(width, height, title, NULL, NULL);
        
        //Set callbacks
        glfwSetWindowUserPointer(platform->nativeWindow, platform);
        glfwSetWindowCloseCallback(platform->nativeWindow, windowClosedCallback);
        glfwSetKeyCallback(platform->nativeWindow, windowKeyCallback);
        glfwSetCursorPosCallback(platform->nativeWindow, windowMousePosCallback);
        glfwSetMouseButtonCallback(platform->nativeWindow, windowMouseButtonCallback);
        
        glfwMakeContextCurrent(platform->nativeWindow);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        platform->windowWidth = width;
        platform->windowHeight = height;

        platform->isKeyDown = fp_isKeyDown;
        platform->isKeyPressed = fp_isKeyPressed;
        platform->isKeyReleased = fp_isKeyReleased;

        platform->isMouseButtonDown = fp_isMouseButtonDown;
        platform->isMouseButtonPressed = fp_isMouseButtonPressed;
        platform->isMouseButtonReleased = fp_isMouseButtonReleased;
    }

    return platform;
}

void salamander_updatePlatform(struct Platform *platform) {
    for (int i = 0; i < INPUT_KEY_BUFFER_SIZE; i++) {
        platform->keyState[i] &= 0b0001;
    }

    for (int i = 0; i < INPUT_BUTTON_BUFFER_SIZE; i++) {
        platform->buttonState[i] &= 0b0001;
    }

    glfwSwapBuffers(platform->nativeWindow);
    glfwPollEvents();
}

//TODO: these dont belong here
Buffer readFileIntoBuffer(char *path) {
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

int findLineInBuffer(Buffer buffer, char *line) {
    const int tempBufferSize = 1024;

    char temp[1024] = { 0 };
    int lastOffset = 0;

    while (true) {
        int offset = 0;
        char *current = buffer.data + lastOffset;

        while (current[offset] != '\n' && current[offset] != '\0') {
            offset++;
        }

        memcpy_s(temp, tempBufferSize, current, offset);
        if (strncmp(temp, line, strnlen(line, tempBufferSize)) == 0) {
            return lastOffset;
        }
        memset(temp, 0, tempBufferSize);

        if (current[offset] == '\0') break;

        lastOffset += ++offset;
    }

    return -1;
}