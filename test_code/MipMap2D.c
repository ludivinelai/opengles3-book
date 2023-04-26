#include <stdlib.h>
#include "esUtil.h"

typedef struct {
    GLuint programObject;
    GLint samplerLoc;
    GLint offsetLoc;
    GLuint textureId;
} UserData;

GLboolean GenMipMap2D(GLubyte *src, GLubyte **dst, int srcWidth, int srcHeight, int *dstWidth, int *dstHeight)
{
    
}

GLubyte *GenCheckImage(int width, int height, int checkSize)
{

}

GLuint CreateMipMappedTexture2D()
{
    GLuint textureId;
    int width = 256, height = 256;
    int level;
    GLubyte *pixels;
    GLubyte *prevImage;
    GLubyte *newImage;

    pixels = GenCheckImage(width, height, 8);
    if (!pixels)
        return 0;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

int Init(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    char vShaderStr[] =
        "#version 300 es                            \n"
        "uniform float u_offset;                    \n"
        "layout(location = 0) in vec4 a_position;   \n"
        "layout(location = 1) in vec2 a_texCoord;   \n"
        "out vec2 v_texCoord;                       \n"
        "void main()                                \n"
        "{                                          \n"
        "    gl_Position = a_position;              \n"
        "    gl_Position.x += u_offset;             \n"
        "    v_texCoord = a_texCoord;               \n"
        "}";
    char fShaderStr[] =
        "#version 300 es                                \n"
        "precision mediump float;                       \n"
        "in vec2 v_texCoord;                            \n"
        "layout(location = 0) out vec4 outColor;        \n"
        "uniform sampler2D s_texture;                   \n"
        "void main()                                    \n"
        "{                                              \n"
        "    outColor = texture(s_texture, v_texCoord); \n"
        "}";
    
    userData->programObject = esLoadProgram(vShaderStr, fShaderStr);
    userData->samplerLoc = glGetUniformLocation(userData->programObject, "s_texture");
    userData->offsetLoc = glGetUniformLocation(userData->programObject, "u_offset");
    userData->textureId = CreateMipMappedTexture2D();

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    return GL_TRUE;
}

void Draw(ESContext *esContext)
{

}

void ShutDown(ESContext *esContext)
{

}

int esMain(ESContext *esContext)
{
    esContext->userData = malloc(sizeof(UserData));

    esCreateWindow(esContext, "MipMap 2D", 320, 240, ES_WINDOW_RGB);

    if (!Init(esContext))
        return GL_FALSE;

    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, ShutDown);

    return GL_TRUE;
}