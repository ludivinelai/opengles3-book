#include <stdlib.h>
#include "esUtil.h"

#define VERTEX_POS_INDEX 0
#define NORMAL_INDEX 1

typedef struct {
    GLuint programObject;
    GLint samplerLoc;
    GLuint textureId;

    int numIndices;
    GLfloat *vertices;
    GLfloat *normals;
    GLfloat *indices;
} UserData;

GLuint CreateSimpleTextureCubemap()
{
    GLuint textureId;
    GLubyte cubePixels[6][3] = {
        255, 0, 0, // Face 0 - Red
        0, 255, 0, // Face 1 - Green,
        0, 0, 255, // Face 2 - Blue
        255, 255, 0, // Face 3 - Yellow
        255, 0, 255, // Face 4 - Purple
        255, 255, 255 // Face 5 - White
    };

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[1]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[2]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[3]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[4]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[5]);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureId;
}

int Init(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    char vShaderStr[] =
        "#version 300 es                            \n"
        "layout(location = 0) in vec4 a_position;   \n"
        "layout(location = 1) in vec3 a_normal;     \n"
        "out vec3 v_normal;                         \n"
        "void main()                                \n"
        "{                                          \n"
        "    gl_Position = a_position;              \n"
        "    v_normal = a_normal;                   \n"
        "}";
    char fShaderStr[] =
        "#version 300 es                                \n"
        "precision mediump float;                       \n"
        "in vec3 v_normal;                              \n"
        "layout(location = 0) out vec4 outColor;        \n"
        "uniform samplerCube s_texture;                 \n"
        "void main()                                    \n"
        "{                                              \n"
        "    outColor = texture(s_texture, v_normal);   \n"
        "}";

    userData->programObject = esLoadProgram(vShaderStr, fShaderStr);
    userData->samplerLoc = glGetUniformLocation(userData->programObject, "s_texture");
    userData->textureId = CreateSimpleTextureCubemap();
    userData->numIndices = esGenSphere(20, 0.75f, &userData->vertices,
                                       &userData->normals, NULL, &userData->indices);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    return GL_TRUE;
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    glDeleteProgram(userData->programObject);
    glDeleteTextures(1, &userData->textureId);

    free(userData->vertices);
    free(userData->normals);
    free(userData->indices); //?
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glUseProgram(userData->programObject);

    glVertexAttribPointer(VERTEX_POS_INDEX, 3, GL_FLOAT, GL_FALSE, 0, userData->vertices);
    glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, 0, userData->normals);
    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glEnableVertexAttribArray(NORMAL_INDEX);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, userData->textureId);

    glUniform1i(userData->samplerLoc, 0);

    glDrawElements(GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices);
}

int esMain(ESContext *esContext)
{
    esContext->userData = malloc(sizeof(UserData));

    esCreateWindow(esContext, "Simple_Texture_Cubemap", 320, 240, ES_WINDOW_RGB);

    if (!Init(esContext))
        return GL_FALSE;

    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, Shutdown);

    return GL_TRUE;
}
