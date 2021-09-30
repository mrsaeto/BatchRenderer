
#include "renderer.h"
#include "platform.h"

#include "glad/glad.h"

static Shader create_shader(const char *vertex_shader_source, const char *fragment_shader_source) {
    Shader shader = { 0 };

    int result;

    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
    if (!result) {
        int log_length;
        char message[1024];
        glGetShaderInfoLog(vertex_shader, 1024, &log_length, message);

        printf("ERROR compiling vertex shader!\n%s\n", message);
    }

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
    if (!result) {
        int log_length;
        char message[1024];
        glGetShaderInfoLog(fragment_shader, 1024, &log_length, message);

        printf("ERROR compiling fragment shader!\n%s\n", message);
    }

    shader.id = glCreateProgram();

    glAttachShader(shader.id, vertex_shader);
    glAttachShader(shader.id, fragment_shader);

    glLinkProgram(shader.id);
    glGetProgramiv(shader.id, GL_LINK_STATUS, &result);
    if (!result) {
        int log_length;
        char message[1024];
        glGetProgramInfoLog(shader.id, 1024, &log_length, message);

        printf("ERROR linking shader!\n%s\n", message);
    }

#ifdef SALAMANDER_DEBUG
    glValidateProgram(shader.id);
    glGetProgramiv(shader.id, GL_VALIDATE_STATUS, &result);
    if (!result) {
        int log_length;
        char message[1024];
        glGetProgramInfoLog(shader.id, 1024, &log_length, message);

        printf("ERROR validating shader!\n%s\n", message);
    }
#endif

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader;
}

Shader load_shader(char *path) {
    Buffer file = read_file_into_buffer(path);
    int copy_size = 0;

    int vs_len = strlen("#VERTEX_SHADER");
    int fs_len = strlen("#FRAGMENT_SHADER");

    int vertex_shader_offset = find_line_in_buffer(file, "#VERTEX_SHADER") + vs_len;
    int fragment_shader_offset = find_line_in_buffer(file, "#FRAGMENT_SHADER") + fs_len;

    int vertex_shader_size = ((fragment_shader_offset - 1) - fs_len) - vertex_shader_offset;    
    char *vertex_shader_source = malloc(vertex_shader_size);
    memset(vertex_shader_source, 0, vertex_shader_size);

    copy_size = vertex_shader_size - 1;
    memcpy_s(vertex_shader_source, copy_size, file.data + vertex_shader_offset, copy_size);

    int fragment_shader_size = file.size - (fragment_shader_offset - 1);
    char *fragment_shader_source = malloc(fragment_shader_size);
    memset(fragment_shader_source, 0, fragment_shader_size);

    copy_size = fragment_shader_size - 1;
    memcpy_s(fragment_shader_source, copy_size, file.data + fragment_shader_offset, copy_size);

    Shader shader = create_shader(vertex_shader_source, fragment_shader_source);

    free(vertex_shader_source);
    free(fragment_shader_source);
    free(file.data);

    return shader;
}

void use_shader(Shader shader) {
    glUseProgram(shader.id);
}

void set_mat4_uniform(Shader shader, char *uniform, mat4 m) {
    int location = glGetUniformLocation(shader.id, uniform);
    glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);
}

typedef struct {
    vec4 position;
    vec4 colour;
} Vertex;

#define FLOATS_PER_VERTEX 4
#define VERTICES_PER_QUAD 4
#define INDICIES_PER_QUAD 6

struct Renderer {
    u32 max_quads_per_batch;

    u32 vao;
    u32 vbo;
    u32 ibo;

    Vertex *buffer;
    u32 quad_count;

    mat4 view_projection;
};

static Renderer g_renderer;

//TODO: remove this copy pasta function
void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

Renderer *create_renderer(int max_quads_per_batch) {
    Renderer *renderer = &g_renderer;
    renderer->max_quads_per_batch = max_quads_per_batch;

    //NOTE: opengl error callback
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );

    glCreateVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glCreateBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    //glEnableVertexAttribArray(0);
    glEnableVertexArrayAttrib(renderer->vbo, 0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, position));

    glCreateBuffers(1, &renderer->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo);

    renderer->buffer = malloc(sizeof(Vertex) * VERTICES_PER_QUAD * max_quads_per_batch);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * VERTICES_PER_QUAD * max_quads_per_batch, NULL, GL_DYNAMIC_DRAW);

    u32 *index_buffer = malloc(sizeof(u32) * INDICIES_PER_QUAD * max_quads_per_batch);
    for (int i = 0, offset = 0; i < INDICIES_PER_QUAD * max_quads_per_batch; i += 6) {
        index_buffer[i + 0] = offset + 0;
        index_buffer[i + 1] = offset + 1;
        index_buffer[i + 2] = offset + 2;

        index_buffer[i + 3] = offset + 2;
        index_buffer[i + 4] = offset + 3;
        index_buffer[i + 5] = offset + 0;

        offset += 4;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * INDICIES_PER_QUAD * max_quads_per_batch, index_buffer, GL_STATIC_DRAW);
    free(index_buffer);



    return renderer;
}

void set_view_projection_matrix(Renderer *renderer, float left, float right, float bottom, float top) {
    mat4 projection;
    glm_ortho(left, right, bottom, top, -1.0f, 1.0f, projection);

    mat4 view;
    glm_mat4_identity(view);

    glm_mat4_mul(projection, view, renderer->view_projection);
}

void get_view_projection_matrix(Renderer *renderer, mat4 m) {
    glm_mat4_copy(renderer->view_projection, m);
}

void clear_renderer() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void add_quad(Renderer *renderer, float x, float y, float width, float height) {
    int offset = renderer->quad_count * VERTICES_PER_QUAD;

    mat4 position;
    glm_mat4_identity(position);
    glm_translate(position, (vec3){ x, y, 0.0f });

    mat4 scale;
    glm_mat4_identity(scale);
    glm_scale(scale, (vec3){ width, height, 1.0f });

    mat4 transform;
    glm_mat4_mul(position, scale, transform);

    //proper screen space geometry
    vec4 quad_vertex_positions[4] = {
        { 0.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f }
    };

    for (int i = 0; i < 4; i++) {
        vec4 p;
        glm_mat4_mulv(transform, quad_vertex_positions[i], p);

        memcpy(renderer->buffer + offset + i, p, sizeof(vec4)); //TODO: is a memcpy as fast just overwriting individual values?

        /*
        renderer->buffer[offset + i].position[0] = p[0];
        renderer->buffer[offset + i].position[1] = p[1];
        renderer->buffer[offset + i].position[2] = p[2];
        renderer->buffer[offset + i].position[3] = p[3];
        */
    }

    renderer->quad_count++;
    if (renderer->quad_count == renderer->max_quads_per_batch) {
        flush_quads(renderer);
    }
}

void flush_quads(Renderer *renderer) {
    int buffer_size = sizeof(Vertex) * VERTICES_PER_QUAD * renderer->quad_count;
    int index_count = INDICIES_PER_QUAD * renderer->quad_count;

    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, renderer->buffer);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, NULL);

    renderer->quad_count = 0;
}