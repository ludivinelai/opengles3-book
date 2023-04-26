#include <stdlib.h>
#include <math.h>
#include"esUtil.h"

#define NUM_INSTANCES   100
#define POSITION_LOC    0
#define COLOR_LOC       1
#define MVP_LOC         2

typedef struct {
    GLuint programObject;

    /* VBOs */
    GLuint positionVBO;
    GLuint colorVBO;
    GLuint mvpVBO;
    GLuint indicesIBO;

    int numIndices; // indices数量

    GLfloat angle[NUM_INSTANCES]; // 选择角度
} UserData;

int Init(ESContext *esContext)
{
    GLfloat *positions;
    GLuint *indices;
    UserData *userData = esContext->userData;
    const char vShaderStr[] = 
        "#version 300 es                            \n"
        "layout(location = 0) in vec4 a_position;   \n"
        "layout(location = 1) in vec4 a_color;      \n"
        "layout(location = 2) in mat4 a_mvpMatrix;  \n"
        "out vec4 v_color;                          \n"
        "void main()                                \n"
        "{                                          \n"
        "    v_color = a_color;                     \n"
        "    gl_Position = a_mvpMatrix * a_position \n"
        "}";
    const char fShaderStr[] =
        "#version 300 es                            \n"
        "precision mediump float;                   \n"
        "in vec4 v_color;                           \n"
        "layout(location = 0) out vec4 outColor;    \n"
        "void main()                                \n"
        "{                                          \n"
        "    outColor = v_color;                    \n"
        "}";

    userData->programObject = esLoadProgram(vShaderStr, fShaderStr);
    // 生成正方体的position数据和indice数据
    // 每个正方体需要24个顶点位置来描述；一个正方体需要12个三角形来描述，所以为36个indices
    userData->numIndices = esGenCube(0.1f, &positions, NULL, NULL, &indices);

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

    // 创建color VBO
    GLubyte colors[NUM_INSTANCES][4] = { 0 };
    int instance;

    srandom(0);

    for (instance = 0; instance < NUM_INSTANCES; instance++) {
        colors[instance][0] = random() % 255;
        colors[instance][1] = random() % 255;
        colors[instance][2] = random() % 255;
        colors[instance][3] = 0;
    }
    glGenBuffers(1, &userData->colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, userData->colorVBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * NUM_INSTANCES * sizeof(GLubyte), colors, GL_STATIC_DRAW);

    // 创建MVP矩阵VBO
    for (instance = 0; instance < NUM_INSTANCES; instance++) {
        userData->angle[instance] = (float)(random() % 32768) / 32767.0f * 360.0f;
    }
    glGenBuffers(1, &userData->mvpVBO);
    glBindBuffer(GL_ARRAY_BUFFER, userData->mvpVBO);
    // 这里matrix数据并没有写入真实数据，会在update函数中更新mvpVBO
    glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * sizeof(ESMatrix), NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
    return GL_TRUE;
}

void Update(ESContext *esContext, float deltaTime)
{
    UserData *userData = (UserData *)esContext->userData;
    ESMatrix *matrixBuf;
    ESMatrix perspective;
    float aspect;
    int instance = 0;
    int numRows;
    int numColumns;

    aspect = (GLfloat)esContext->width / (GLfloat)esContext->height;

    esMatrixLoadIdentity(&perspective);
    esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);

    glBindBuffer(GL_ARRAY_BUFFER, userData->mvpVBO);
    matrixBuf = (ESMatrix *)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(ESMatrix) * NUM_INSTANCES, GL_MAP_WRITE_BIT);

    numRows = (int)sqrtf(NUM_INSTANCES);
    numColumns = numRows;

    for (instance = 0; instance < NUM_INSTANCES; instance++) {
        ESMatrix modelview;
        float translateX = ((float)(instance % numRows) / (float)numRows) * 2.0f - 1.0f;
        float translateY = ((float)(instance / numColumns) / (float)numColumns) * 2.0f - 1.0f;

        esMatrixLoadIdentity(&modelview);
        esTranslate(&modelview, translateX, translateY, -2.0f);

        userData->angle[instance] += deltaTime * 40.0f;
        if (userData->angle[instance] > 360.0f)
            userData->angle[instance] -= 360.0f;

        esRotate(&modelview, userData->angle[instance], 1.0, 0.0, 1.0);

        esMatrixMultiply(&matrixBuf[instance], &modelview, &perspective);
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(userData->programObject);

    glBindBuffer(GL_ARRAY_BUFFER, userData->positionVBO);
    glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)NULL);
    glEnableVertexAttribArray(POSITION_LOC);

    glBindBuffer(GL_ARRAY_BUFFER, userData->colorVBO);
    glVertexAttribPointer(COLOR_LOC, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(GLubyte), (const void *)NULL);
    glEnableVertexAttribArray(COLOR_LOC);
    glVertexAttribDivisor(COLOR_LOC, 1);

    glBindBuffer(GL_ARRAY_BUFFER, userData->mvpVBO);

    glVertexAttribPointer(MVP_LOC, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)NULL);
    glVertexAttribPointer(MVP_LOC + 1, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)(sizeof(GLfloat) * 4));
    glVertexAttribPointer(MVP_LOC + 2, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)(sizeof(GLfloat) * 8));
    glVertexAttribPointer(MVP_LOC + 3, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)(sizeof(GLfloat) * 12));

    glEnableVertexAttribArray(MVP_LOC);
    glEnableVertexAttribArray(MVP_LOC + 1);
    glEnableVertexAttribArray(MVP_LOC + 2);
    glEnableVertexAttribArray(MVP_LOC + 3);

    glVertexAttribDivisor(MVP_LOC, 1);
    glVertexAttribDivisor(MVP_LOC + 1, 1);
    glVertexAttribDivisor(MVP_LOC + 2, 1);
    glVertexAttribDivisor(MVP_LOC + 3, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->indicesIBO);
    glDrawElementsInstanced(GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, (const void *)NULL, NUM_INSTANCES);
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    glDeleteBuffers(1, &userData->colorVBO);
    glDeleteBuffers(1, &userData->indicesIBO);
    glDeleteBuffers(1, &userData->mvpVBO);
    glDeleteBuffers(1, &userData->positionVBO);

    glDeleteProgram(userData->programObject);
}

int esMain(ESContext *esContext)
{
    esContext->userData = malloc ( sizeof ( UserData ) );

    esCreateWindow ( esContext, "Instancing", 640, 480, ES_WINDOW_RGB | ES_WINDOW_DEPTH );

    if ( !Init ( esContext ) )
        return GL_FALSE;

    esRegisterShutdownFunc ( esContext, Shutdown );
    esRegisterUpdateFunc ( esContext, Update );
    esRegisterDrawFunc ( esContext, Draw );

    return GL_TRUE;
}