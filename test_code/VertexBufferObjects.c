#include "esUtil.h"

typedef struct
{
    GLuint programObject;
    GLuint vboIds[2];
    GLuint offsetLoc;
} UserData;

#define VERTEX_POS_SIZE 3
#define VERTEX_COLOR_SIZE 4

#define VERTEX_POS_INDEX 0
#define VERTEX_COLOR_INDEX 1

int Init(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    const char vShaderStr[] =
        "#version 300 es                            \n"
        "layout(location = 0) in vec4 a_position;   \n"
        "layout(location = 1) in vec4 a_color;      \n"
        "uniform float u_offset;                    \n"
        "out vec4 v_color;                          \n"
        "void main()                                \n"
        "{                                          \n"
        "   v_color = a_color;                      \n"
        "   gl_Position = a_position;               \n"
        "   gl_Position.x += u_offset;              \n"
        "}";
    const char fShaderStr[] =
        "#version 300 es            \n"
        "precision mediump float;   \n"
        "in vec4 v_color;           \n"
        "out vec4 o_fragColor;      \n"
        "void main()                \n"
        "{                          \n"
        "   o_fragColor = v_color;  \n"
        "}";
    GLuint programObject = esLoadProgram(vShaderStr, fShaderStr);

    userData->offsetLoc = glGetUniformLocation(programObject, "u_offset");

    if (programObject == 0)
        return GL_FALSE;

    userData->programObject = programObject;
    userData->vboIds[0] = 0;
    userData->vboIds[1] = 0;

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

/*
 * vertices: 指向顶点属性数据buffer的指针
 * vtxStride: 每个顶点属性数据的步长（字节）
 * numIndices：使用三角形绘制图元所需的索引数
 * indices：指向element index buffer的指针
 */
void DrawPrimitiveWithoutVBOs(GLfloat *vertices,
                              GLint vtxStride,
                              GLint numIndices,
                              GLushort *indices)
{
    GLfloat *vtxBuf = vertices;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDEX);

    glVertexAttribPointer(VERTEX_POS_INDEX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);
    vtxBuf += VERTEX_POS_SIZE;
    glVertexAttribPointer(VERTEX_COLOR_INDEX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);

    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indices);

    glDisableVertexAttribArray(VERTEX_POS_INDEX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDEX);
}

void DrawPrimitiveWithVBOs(ESContext *esContext,
                           GLint numVertices, GLfloat *vtxBuf, GLint vtxStride,
                           GLint numIndices, GLushort *indices)
{
    UserData *userData = esContext->userData;
    GLuint offset = 0;
    GLboolean firstDraw = FALSE;

    if (userData->vboIds[0] == 0 && userData->vboIds[1] == 0)
        firstDraw = TRUE;

    if (firstDraw)
        glGenBuffers(2, userData->vboIds);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);

    if (firstDraw) {
        glBufferData(GL_ARRAY_BUFFER, vtxStride * numVertices, vtxBuf, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numIndices, indices, GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDEX);

    glVertexAttribPointer(VERTEX_POS_INDEX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, vtxStride, (const void *)offset);
    offset += VERTEX_POS_SIZE * sizeof(GLfloat);
    glVertexAttribPointer(VERTEX_COLOR_INDEX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, vtxStride, (const void *)offset);

    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(VERTEX_POS_INDEX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDEX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] = {
        -0.5f, 0.5f, 0.0f,      // v0
        1.0f, 0.0f, 0.0f, 1.0f, // c0
        -1.0f, -0.5f, 0.0f,     // v1
        0.0f, 1.0f, 0.0f, 1.0f, // c1
        0.0f, -0.5f, 0.0f,      // v2
        0.0f, 0.0f, 1.0f, 1.0f, // c2
    };
    GLushort indices[3] = {0, 1, 2};

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->programObject);

    glUniform1f(userData->offsetLoc, 0.0f);

    DrawPrimitiveWithoutVBOs(vertices, sizeof(GLfloat) * (VERTEX_COLOR_SIZE + VERTEX_POS_SIZE), 3, indices);

    glUniform1f(userData->offsetLoc, 1.0f);

    DrawPrimitiveWithVBOs(esContext, 3, vertices, sizeof(GLfloat) * (VERTEX_COLOR_SIZE + VERTEX_POS_SIZE), 3, indices);
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    glDeleteProgram(userData->programObject);
    glDeleteBuffers(2, userData->vboIds);
}

int esMain(ESContext *esContext)
{
    esContext->userData = malloc(sizeof(UserData));

    esCreateWindow(esContext, "VertexBufferObjects", 320, 240, ES_WINDOW_RGB);

    if (!Init(esContext))
        return GL_FALSE;

    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, Shutdown);

    return GL_TRUE;
}
