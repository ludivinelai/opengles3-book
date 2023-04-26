#include <stdlib.h>
#include <math.h>
#include"esUtil.h"

#define USER_CLIP_PLANE_ENABLE  1
#define POSITION_LOC            0

typedef struct {
    GLuint programObject;

    /* VBOs */
    GLuint positionVBO;
    GLuint indicesIBO;

    /* uniform locations */
    GLint  mvpLoc;
    GLint clipPlaneLoc;

    int numIndices; // indices数量

    ESMatrix mvpMatrix;
    GLfloat clipPlaneVec[4];
} UserData;

int InitMVP(ESContext *esContext)
{
   ESMatrix perspective;
   ESMatrix modelview;
   float aspect;
   UserData *userData = esContext->userData;
   GLfloat planeVector[4] = {1.0f, 0.0f, 0.0f, 0.0f};

   // Compute the window aspect ratio
   aspect = (GLfloat)esContext->width / (GLfloat)esContext->height;

   // Generate a perspective matrix with a 60 degree FOV
   esMatrixLoadIdentity(&perspective);
   esPerspective(&perspective, 60.0f, aspect, 0.1f, 20.0f);

   for (int i = 0; i < 4; i++) {
       for (int j = 0; j < 4; j++) {
           esLogMessage("%f ", perspective.m[i][j]);
       }
       esLogMessage("\n");
   }

   // Generate a model view matrix to rotate/translate the terrain
   esMatrixLoadIdentity(&modelview);

   // Center the object
   esTranslate(&modelview, 0.0f, 0.0f, -2.0f);

   // Rotate
   esRotate(&modelview, 45.0f, 1.0, 0.0, 1.0);

   // Compute the final MVP by multiplying the
   // modelview and perspective matrices together
   esMatrixMultiply(&userData->mvpMatrix, &modelview, &perspective);
   memcpy(userData->clipPlaneVec, planeVector, sizeof(planeVector));

   return TRUE;
}

int Init(ESContext *esContext)
{
    GLfloat *positions = NULL;
    GLuint *indices = NULL;
    UserData *userData = esContext->userData;
    const char vShaderStr[] = 
        "#version 300 es                                        \n"
        "uniform vec4 u_clipPlane;                              \n"
        "uniform mat4 u_mvpMatrix;                              \n"
        "layout(location = 0) in vec4 vPosition;                \n"
        "out float v_clipDist;                                  \n"
        "void main()                                            \n"
        "{                                                      \n"
        // "   mat4 u_mvpMatrix = mat4(1.0);           \n"
        "   v_clipDist = dot(vPosition.xyz, u_clipPlane.xyz) +  \n"
        "                u_clipPlane.w;                         \n"
        "   gl_Position = vPosition;              \n"
        "}";
    const char fShaderStr[] =
        "#version 300 es                            \n"
        "precision mediump float;                   \n"
        "in float v_clipDist;                       \n"
        "layout(location = 0) out vec4 outColor;    \n"
        "void main()                                \n"
        "{                                          \n"
#if USER_CLIP_PLANE_ENABLE
        "    if (v_clipDist < 0.0)                  \n"
        "        discard;                           \n"
#endif
        "    outColor = vec4(1.0, 0.0, 0.0, 1.0);   \n"
        "}";

    userData->programObject = esLoadProgram(vShaderStr, fShaderStr);
    
    // Get the uniform locations
    userData->mvpLoc = glGetUniformLocation(userData->programObject, "u_mvpMatrix");
    userData->clipPlaneLoc = glGetUniformLocation(userData->programObject, "u_clipPlane");
    
    // 生成正方体的position数据和indice数据
    // 每个正方体需要24个顶点位置来描述；一个正方体需要12个三角形来描述，所以为36个indices
    userData->numIndices = esGenCube(1.0f, &positions, NULL, NULL, &indices);

    // 创建单一正方体的indices VBO
    glGenBuffers(1, &userData->indicesIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->indicesIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * (userData->numIndices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    free(indices);

    // 创建单一正方体的positions VBO
    glGenBuffers(1, &userData->positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, userData->positionVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * 3 * sizeof(GLfloat), positions, GL_STATIC_DRAW);
    free(positions);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    return GL_TRUE;
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;

#if 1
    InitMVP(esContext);
#else
    esMatrixLoadIdentity(&userData->mvpMatrix);
#endif

    GLfloat vVertices[] = { 0.0f,  0.5f, 0.0f,
                         -0.5f, -0.5f, 0.0f,
                         0.5f, -0.5f, 0.0f
    };

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(userData->programObject);

#if 0
    glBindBuffer(GL_ARRAY_BUFFER, userData->positionVBO);
    glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)NULL);
    glEnableVertexAttribArray(POSITION_LOC);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->indicesIBO);
#endif
    // Load the MVP matrix
    glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, (GLfloat *)&userData->mvpMatrix.m[0][0]);

    glUniform4fv(userData->clipPlaneLoc, 1, userData->clipPlaneVec);

    //glDrawElements(GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, (const void *)NULL);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    // glDeleteBuffers(1, &userData->colorVBO);
    // glDeleteBuffers(1, &userData->mvpVBO);
    glDeleteBuffers(1, &userData->indicesIBO);
    glDeleteBuffers(1, &userData->positionVBO);

    glDeleteProgram(userData->programObject);
}

int esMain(ESContext *esContext)
{
    esContext->userData = malloc(sizeof(UserData));
    char esTitle[] =
#if USER_CLIP_PLANE_ENABLE
        "With User Clip Plane";
#else
        "Without User Clip Plane";
#endif

    esCreateWindow(esContext, esTitle, 480, 480, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

    if (!Init(esContext))
        return GL_FALSE;

    esRegisterShutdownFunc(esContext, Shutdown);
    esRegisterDrawFunc(esContext, Draw);

    return GL_TRUE;
}