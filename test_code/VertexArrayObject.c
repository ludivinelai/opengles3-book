#include "esUtil.h"

typedef struct {
    GLuint programObject;
    GLuint vboIds[2];
    GLuint vaoId;
} UserData;

#define VERTEX_POS_SIZE     3
#define VERTEX_COLOR_SIZE   4

#define VERTEX_POS_INDEX    0
#define VERTEX_COLOR_INDEX  1

#define VERTEX_STRIDE (sizeof(GLfloat) * (VERTEX_COLOR_SIZE + VERTEX_POS_SIZE))

int Init(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    const char vShaderStr[] =
        "#version 300 es                            \n"
        "layout(location = 0) in vec4 a_position;   \n"
        "layout(location = 1) in vec4 a_color;      \n"
        "out vec4 v_color;                          \n"
        "void main()                                \n"
        "{                                          \n"
        "    v_color = a_color;                     \n"
        "    gl_Position = a_position;              \n"
        "}";
    const char fShaderStr[] =
        "#version 300 es            \n"
        "precision mediump float;   \n"
        "in vec4 v_color;           \n"
        "out vec4 o_fragColor;      \n"
        "void main()                \n"
        "{                          \n"
        "    o_fragColor = v_color; \n"
        "}" ;
    GLuint programObject;
    GLfloat vertices[3 * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE )] =
    {
        0.0f,  0.5f, 0.0f,        // v0
        1.0f,  0.0f, 0.0f, 1.0f,  // c0
        -0.5f, -0.5f, 0.0f,       // v1
        0.0f,  1.0f, 0.0f, 1.0f,  // c1
        0.5f, -0.5f, 0.0f,        // v2
        0.0f,  0.0f, 1.0f, 1.0f,  // c2
    };
    GLushort indices[3] = { 0, 1, 2 };

    programObject = esLoadProgram(vShaderStr, fShaderStr);
    if (programObject == 0)
        return GL_FALSE;

    userData->programObject = programObject;

    glGenBuffers(2, userData->vboIds);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &userData->vaoId);

    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);

    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDEX);

    glVertexAttribPointer(VERTEX_POS_INDEX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *)0);
    glVertexAttribPointer(VERTEX_COLOR_INDEX, VERTEX_COLOR_SIZE,
                          GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
                          (const void *)(sizeof(GLfloat) * VERTEX_POS_SIZE));

    glBindVertexArray(0);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram ( userData->programObject );
    glBindVertexArray(userData->vaoId);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (const void *)0);
    glBindVertexArray(0);
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    
    glDeleteProgram(userData->programObject);
    glDeleteBuffers(2, userData->vboIds);
    glDeleteVertexArrays(1, &userData->vaoId);
}

int esMain(ESContext *esContext)
{
    esContext->userData = malloc(sizeof(UserData));

    esCreateWindow(esContext, "VertexArrayObjects", 320, 240, ES_WINDOW_RGB);

    if (!Init(esContext))
        return GL_FALSE;

    esRegisterShutdownFunc(esContext, Shutdown);
    esRegisterDrawFunc(esContext, Draw);

    return GL_TRUE;
}