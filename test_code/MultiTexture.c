#include <stdlib.h>
#include "esUtil.h"

typedef struct {
    GLuint programObject;

    GLint baseMapLoc;
    GLint lightMapLoc;

    GLuint baseMapTexId;
    GLuint lightMapTexId;
} UserData;

GLuint LoadTexture(void* ioContext, char* filename)
{
    int width, height;
    char* buffer = esLoadTGA(ioContext, filename, &width, &height);
    GLuint texId;

    if (!buffer) {
        esLogMessage("Error loading %s image.\n", filename);
        return 0;
    }

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return texId;
}

int Init(ESContext* esContext)
{
    UserData* userData = esContext->userData;
    char vShaderStr[] =
        "#version 300 es                            \n"
        "layout(location = 0) in vec4 a_position;   \n"
        "layout(location = 1) in vec2 a_texCoord;   \n"
        "out vec2 v_texCoord;                       \n"
        "void main()                                \n"
        "{                                          \n"
        "    gl_Position = a_position;              \n"
        "    v_texCoord = a_texCoord;               \n"
        "}";
    char fShaderStr[] =
        "#version 300 es                                    \n"
        "precision mediump float;                           \n"
        "in vec2 v_texCoord;                                \n"
        "layout(location = 0) out vec4 outColor;            \n"
        "uniform sampler2D s_baseMap;                       \n"
        "uniform sampler2D s_lightMap;                      \n"
        "void main()                                        \n"
        "{                                                  \n"
        "    vec4 baseColor;                                \n"
        "    vec4 lightColor;                               \n"
        "                                                   \n"
        "    baseColor = texture(s_baseMap, v_texCoord);    \n"
        "    lightColor = texture(s_lightMap, v_texCoord);  \n"
        "                                                   \n"
        "    outColor = baseColor * (lightColor + 0.25);    \n"
        "}";

    userData->programObject = esLoadProgram(vShaderStr, fShaderStr);

    userData->baseMapLoc = glGetUniformLocation(userData->programObject, "s_baseMape");
    userData->lightMapLoc = glGetUniformLocation(userData->programObject, "s_lightMap");

    userData->baseMapTexId = LoadTexture(esContext->platformData, "basemap.tga");
    userData->lightMapTexId = LoadTexture(esContext->platformData, "lightmap.tga");
    if (userData->baseMapTexId == 0 || userData->lightMapTexId == 0)
        return GL_FALSE;

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

void Draw(ESContext* esContext)
{
    UserData* userData = esContext->userData;
    GLfloat vVertices[] = {
        -0.5f, 0.5f, 0.0f,
        0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,
        1.0f, 1.0f,
        0.5f, 0.5f, 0.0f,
        1.0f, 0.0f
    };
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(userData->programObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vVertices);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vVertices + 3);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->baseMapTexId);
    glUniform1i(userData->baseMapLoc, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, userData->lightMapTexId);
    glUniform1i(userData->lightMapLoc, 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

void Shutdown(ESContext* esContext)
{
    UserData* userData = esContext->userData;

    glDeleteTextures(1, &userData->baseMapTexId);
    glDeleteTextures(1, &userData->lightMapTexId);
    glDeleteProgram(userData->programObject);
}

int esMain(ESContext* esContext)
{
    esContext->userData = malloc(sizeof(UserData));
    esCreateWindow(esContext, "MultiTexture", 320, 240, ES_WINDOW_RGB);
    if (!Init(esContext))
        return GL_FALSE;

    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, Shutdown);

    return GL_TRUE;
}