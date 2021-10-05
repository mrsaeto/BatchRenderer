#include "renderer.h"
#include "platform.h"

#include "glad/glad.h"

static struct Shader createShader(const char *vertexShaderSource, const char *fragmentShaderSource) {
    struct Shader shader = { 0 };
    int result;

    if (vertexShaderSource && fragmentShaderSource) {

        u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
        if (!result) {
            int logLength;
            char message[1024];
            glGetShaderInfoLog(vertexShader, 1024, &logLength, message);

            printf("ERROR compiling vertex shader!\n%s\n", message);
        }

        u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
        if (!result) {
            int logLength;
            char message[1024];
            glGetShaderInfoLog(fragmentShader, 1024, &logLength, message);

            printf("ERROR compiling fragment shader!\n%s\n", message);
        }

        shader.id = glCreateProgram();

        glAttachShader(shader.id, vertexShader);
        glAttachShader(shader.id, fragmentShader);

        glLinkProgram(shader.id);
        glGetProgramiv(shader.id, GL_LINK_STATUS, &result);
        if (!result) {
            int logLength;
            char message[1024];
            glGetProgramInfoLog(shader.id, 1024, &logLength, message);

            printf("ERROR linking shader!\n%s\n", message);
        }

#ifdef SALAMANDER_DEBUG
        glValidateProgram(shader.id);
        glGetProgramiv(shader.id, GL_VALIDATE_STATUS, &result);
        if (!result) {
            int logLength;
            char message[1024];
            glGetProgramInfoLog(shader.id, 1024, &logLength, message);

            printf("ERROR validating shader!\n%s\n", message);
        }
#endif

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

    }

    return shader;
}

struct Shader salamander_loadShader(char *path) {
    Buffer file = readFileIntoBuffer(path);
    int copySize = 0;

    int vsLen = strlen("#VERTEX_SHADER");
    int fsLen = strlen("#FRAGMENT_SHADER");

    int vertexShaderOffset = findLineInBuffer(file, "#VERTEX_SHADER") + vsLen;
    int fragmentShaderOffset = findLineInBuffer(file, "#FRAGMENT_SHADER") + fsLen;

    char *vertexShaderSource = NULL;
    char *fragmentShaderSource = NULL;

    if (vertexShaderOffset != -1 && fragmentShaderOffset != -1) {

        int vertexShaderSize = ((fragmentShaderOffset - 1) - fsLen) - vertexShaderOffset;
        vertexShaderSource = malloc(vertexShaderSize);
        memset(vertexShaderSource, 0, vertexShaderSize);

        copySize = vertexShaderSize - 1;
        memcpy_s(vertexShaderSource, copySize, file.data + vertexShaderOffset, copySize);

        int fragmentShaderSize = file.size - (fragmentShaderOffset - 1);
        fragmentShaderSource = malloc(fragmentShaderSize);
        memset(fragmentShaderSource, 0, fragmentShaderSize);

        copySize = fragmentShaderSize - 1;
        memcpy_s(fragmentShaderSource, copySize, file.data + fragmentShaderOffset, copySize);

    }

    struct Shader shader = createShader(vertexShaderSource, fragmentShaderSource);

    free(vertexShaderSource);
    free(fragmentShaderSource);
    free(file.data);

    return shader;
}

void salamander_useShader(struct Shader shader) {
    glUseProgram(shader.id);
}

void salamander_setShaderMat4(struct Shader shader, char *uniform, mat4 matrix) {
    int location = glGetUniformLocation(shader.id, uniform);
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

struct Vertex {
    vec4 position;
    vec4 colour;

    vec2 textureCoordinates;
    float textureIndex;
};

#define VERTICES_PER_QUAD 4
#define INDICIES_PER_QUAD 6

#define RENDERER_TEXTURE_SLOTS 32
struct Renderer {
    u32 maxQuadsPerBatch;

    u32 vao;
    u32 vbo;
    u32 ibo;

    struct Vertex *buffer;
    u32 currentQuadCount;

    struct Texture textureSlots[RENDERER_TEXTURE_SLOTS];
    int currentTextureIndex;
};

static struct Renderer g_renderer;

#ifdef SALAMANDER_DEBUG
void errorCallback(u32 source, u32 type, u32 id, u32 severity, int length, const char* message, const void* userParam) {
    char *typeString = (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "");
    printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", typeString, type, severity, message );
}
#endif

struct Renderer *salamander_createRenderer(int maxQuadsPerBatch) {
    struct Renderer *renderer = &g_renderer;
    renderer->maxQuadsPerBatch = maxQuadsPerBatch;

#ifdef SALAMANDER_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(errorCallback, 0);
#endif

    glCreateVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glCreateBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    glEnableVertexArrayAttrib(renderer->vbo, 0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const void *)offsetof(struct Vertex, position));

    glEnableVertexArrayAttrib(renderer->vbo, 1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const void *)offsetof(struct Vertex, colour));

    glEnableVertexArrayAttrib(renderer->vbo, 2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const void *)offsetof(struct Vertex, textureCoordinates));

    glEnableVertexArrayAttrib(renderer->vbo, 3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const void *)offsetof(struct Vertex, textureIndex));

    glCreateBuffers(1, &renderer->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo);

    renderer->buffer = malloc(sizeof(struct Vertex) * VERTICES_PER_QUAD * maxQuadsPerBatch);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct Vertex) * VERTICES_PER_QUAD * maxQuadsPerBatch, NULL, GL_DYNAMIC_DRAW);

    u32 *indexBuffer = malloc(sizeof(u32) * INDICIES_PER_QUAD * maxQuadsPerBatch);
    for (int i = 0, offset = 0; i < INDICIES_PER_QUAD * maxQuadsPerBatch; i += 6) {
        indexBuffer[i + 0] = offset + 0;
        indexBuffer[i + 1] = offset + 1;
        indexBuffer[i + 2] = offset + 2;

        indexBuffer[i + 3] = offset + 2;
        indexBuffer[i + 4] = offset + 3;
        indexBuffer[i + 5] = offset + 0;

        offset += 4;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * INDICIES_PER_QUAD * maxQuadsPerBatch, indexBuffer, GL_STATIC_DRAW);
    free(indexBuffer);

    return renderer;
}

void salamander_clearRenderer(vec4 colour) {
    glClearColor(colour[0], colour[1], colour[2], colour[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void salamander_drawQuad(struct Renderer *renderer, vec2 position, vec2 size, vec4 colour) {
    int offset = renderer->currentQuadCount * VERTICES_PER_QUAD;

    mat4 positionMatrix;
    glm_mat4_identity(positionMatrix);
    glm_translate(positionMatrix, (vec3){ position[0], position[1], 0.0f });

    mat4 scaleMatrix;
    glm_mat4_identity(scaleMatrix);
    glm_scale(scaleMatrix, (vec3){ size[0], size[1], 1.0f });

    mat4 transform;
    glm_mat4_mul(positionMatrix, scaleMatrix, transform);

    //proper screen space geometry
    vec4 quadVertexPositions[4] = {
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 1.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f, 1.0f }
    };

    for (int i = 0; i < 4; i++) {
        vec4 p;
        glm_mat4_mulv(transform, quadVertexPositions[i], p);

        //NOTE: a bit verbose but makes more sense than a memcpy
        renderer->buffer[offset + i].position[0] = p[0];
        renderer->buffer[offset + i].position[1] = p[1];
        renderer->buffer[offset + i].position[2] = p[2];
        renderer->buffer[offset + i].position[3] = p[3];

        renderer->buffer[offset + i].colour[0] = colour[0];
        renderer->buffer[offset + i].colour[1] = colour[1];
        renderer->buffer[offset + i].colour[2] = colour[2];
        renderer->buffer[offset + i].colour[3] = colour[3];

        renderer->buffer[offset + i].textureIndex = -1.0f;
    }

    renderer->currentQuadCount++;
    if (renderer->currentQuadCount == renderer->maxQuadsPerBatch) {
        salamander_flushRenderer(renderer);
    }
}

void salamander_drawTexture(struct Renderer *renderer, struct Texture texture, vec2 position, vec2 scale) {
    int offset = renderer->currentQuadCount * VERTICES_PER_QUAD;

    mat4 positionMatrix;
    glm_mat4_identity(positionMatrix);
    glm_translate(positionMatrix, (vec3){ position[0], position[1], 0.0f });

    mat4 scaleMatrix;
    glm_mat4_identity(scaleMatrix);
    glm_scale(scaleMatrix, (vec3){ texture.width * scale[0], texture.height * scale[1], 1.0f });

    mat4 transform;
    glm_mat4_mul(positionMatrix, scaleMatrix, transform);

    //proper screen space geometry
    vec4 quadVertexPositions[4] = {
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 1.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f, 1.0f }
    };

    vec2 quadTextureCoordinates[4] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
    };

    //TODO: do we want to use a pointer to the texture or just pass the whole struct?
    float textureIndex = -1.0f;
    for (int i = 0; i < RENDERER_TEXTURE_SLOTS; i++) {
        if (renderer->textureSlots[i].id == texture.id) {
            textureIndex = (float)i;
        }
    }

    if (textureIndex == -1.0f) {
        textureIndex = (float)renderer->currentTextureIndex;
        renderer->textureSlots[renderer->currentTextureIndex++] = texture;
    }

    for (int i = 0; i < 4; i++) {
        vec4 p;
        glm_mat4_mulv(transform, quadVertexPositions[i], p);

        //NOTE: a bit verbose but makes more sense than a memcpy
        renderer->buffer[offset + i].position[0] = p[0];
        renderer->buffer[offset + i].position[1] = p[1];
        renderer->buffer[offset + i].position[2] = p[2];
        renderer->buffer[offset + i].position[3] = p[3];

        renderer->buffer[offset + i].colour[0] = 1.0f;
        renderer->buffer[offset + i].colour[1] = 1.0f;
        renderer->buffer[offset + i].colour[2] = 1.0f;
        renderer->buffer[offset + i].colour[3] = 1.0f;

        renderer->buffer[offset + i].textureCoordinates[0] = quadTextureCoordinates[i][0];
        renderer->buffer[offset + i].textureCoordinates[1] = quadTextureCoordinates[i][1];

        renderer->buffer[offset + i].textureIndex = textureIndex;
    }

    renderer->currentQuadCount++;
    if (renderer->currentQuadCount == renderer->maxQuadsPerBatch) {
        salamander_flushRenderer(renderer);
    }
}

void salamander_flushRenderer(struct Renderer *renderer) {
    int bufferSize = sizeof(struct Vertex) * VERTICES_PER_QUAD * renderer->currentQuadCount;
    int elementCount = INDICIES_PER_QUAD * renderer->currentQuadCount;

    for (int i = 0; i < renderer->currentTextureIndex; i++) {
        struct Texture texture = renderer->textureSlots[i];
        glBindTextureUnit(i, texture.id);
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, renderer->buffer);
    glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, NULL);

    renderer->currentQuadCount = 0;
    renderer->currentTextureIndex = 0;
}