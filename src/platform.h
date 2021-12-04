#ifndef SALAMANDER_PLATFORM_H
#define SALAMANDER_PLATFORM_H

#include "basic.h"

#define INPUT_KEY_BUFFER_SIZE 128
#define INPUT_BUTTON_BUFFER_SIZE 3

struct Platform {
    void *nativeWindow;
    
    int windowWidth;
    int windowHeight;
    
    bool windowClosed;
    
    //Input state
    u8 keyState[INPUT_KEY_BUFFER_SIZE];
    u8 buttonState[INPUT_BUTTON_BUFFER_SIZE];
    
    float mouseX;
    float mouseY;
    
    //Polling functions
    bool (*isKeyDown)(int);
    bool (*isKeyPressed)(int);
    bool (*isKeyReleased)(int);
    
    bool (*isMouseButtonDown)(int);
    bool (*isMouseButtonPressed)(int);
    bool (*isMouseButtonReleased)(int);
};

struct Platform *createPlatform(char *title, int width, int height);
void updatePlatform(struct Platform *platform);

typedef struct {
    int size;
    u8 *data;
} Buffer;

Buffer readFileIntoBuffer(char *path);
int findLineInBuffer(Buffer buffer, char *line);

#endif