#include <stdlib.h>
#include "esUtil.h"

typedef struct {
    GLuint programObject;

    GLint mvpLoc;

    GLfloat *vertices;
    GLuint *indices;
    int numIndices;

    GLfloat angle;
    ESMatrix mvpMatrix;
} UserData;

int Init(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    const char vShaderStr[] =
        #version 300 es
        uniform mat4 u_mvpMatrix;

}

void Draw(ESContext *esContext)
{

}

void Update(ESContext *esContext)
{

}

void Shutdown(ESContext *esContext)
{

}

int esMain(ESContext *esContext)
{
    esContext->userData = malloc(sizeof(UserData));
    esCreateWindow(esContext, "Simple Vertex Shader", 320, 240, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

    if (!Init(esContext))
        return GL_FALSE;

    esRegisterDrawFunc(esContext, Draw);
    esRegisterUpdateFunc(esContext, Update);
    esRegisterShutdownFunc(esContext, Shutdown);

    return GL_TRUE;
}