#ifndef SALAMANDER_PLATFORM_H
#define SALAMANDER_PLATFORM_H

#include "basic.h"

#define INPUT_KEY_BUFFER_SIZE 128
typedef struct {
    void *native_window;

    int window_width;
    int window_height;

    bool window_closed;

    u8 key_state[INPUT_KEY_BUFFER_SIZE];

    float mouse_x;
    float mouse_y;
} Platform;

Platform *create_platform(char *title, int width, int height);
void update_platform(Platform *platform);

typedef struct {
    int size;
    u8 *data;
} Buffer;

Buffer read_file_into_buffer(char *path);
int find_line_in_buffer(Buffer file, char *line);

#endif