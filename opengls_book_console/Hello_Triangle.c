/****************************************************************
file:         hello_Triangle.c
description:  the source file opengles draw triangle
date:         2022/05/018
author        hufengkai
****************************************************************/
#include "esUtil.h"
#include "log.h"
#include "config.h"
#define HELLOE_TRIANGLE

#ifdef HELLOE_TRIANGLE
//#define ORTH
#define VAO

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     3 // r, g, b, and a
#define VERTEX_TEX_COR_SIZE     2 // u,v

#define VERTEX_LEN  ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE +VERTEX_TEX_COR_SIZE)

#define VETERX_FACE             4
#define VERTEX_POS_INDX         0
#define VERTEX_COLOR_INDX       1 
#define VERTEX_TEX_COR_INDX     2 

#define VERTEX_STRIDE         ( sizeof(GLfloat) * (VERTEX_LEN) )

typedef struct
{
    // Handle to a program object
    GLuint programObject;

    // Uniform locations
    GLint  mvpLoc;
    GLint  scaleLoc;


    // Vertex daata
    GLfloat* vertices;
    GLuint* indices;
    int       numIndices;

    // Rotation angle
    GLfloat   angle;
    GLfloat   angle_x;
    GLfloat   angle_y;
    GLfloat   angle_z;
    GLfloat   angle_f;

    GLfloat   scale;

    GLfloat   z_pos;
    unsigned int texture1;

    // VertexBufferObject Ids
    GLuint vboIds[2];

    // VertexArrayObject Id
    GLuint vaoId;

    // MVP matrix
    ESMatrix  mvpMatrix;
    ESMatrix perspective;
    ESMatrix model;
    ESMatrix view;

} UserData;

void onKeyPressed(ESContext* esContext, unsigned char key, int x, int y) {
    log_d(" key %c x=%d ,y=%d ", key, x, y);
    UserData* userData = esContext->userData;

    switch (key)
    {
    case 'a':
        userData->angle_y += 1.0f;
        if (userData->angle_y >= 360.0f)
        {
            userData->angle_y -= 360.0f;
        }

        break;
    case 'w':

        userData->angle_x += 1.0f;
        if (userData->angle_x >= 360.0f)
        {
            userData->angle_x -= 360.0f;
        }


        break;
    case 's':

        userData->angle_x -= 1.0f;
        if (userData->angle_x <= -360.0f)
        {
            userData->angle_x = 360.0f;
        }


        break;
    case 'd':

        userData->angle_y -= 1.0f;
        if (userData->angle_y <= -360.0f)
        {
            userData->angle_y = 360.0f;
        }



        break;
    case 'f':
#ifdef ORTH

        userData->scale += 0.01;
        if (userData->scale >= 3.0) {
            userData->scale = 0.5;
        }
#else
        userData->angle_f += 1;
        if (userData->angle_f >= 90.0) {
            userData->angle_f = 0.0;
        }

#endif

        break;
    case 'n':
#ifdef ORTH

        userData->scale -= 0.01;
        if (userData->scale <= 0.5) {
            userData->scale = 3.0;
        }
#else
        userData->angle_f -= 1;
        if (userData->angle_f <= 0.0) {
            userData->angle_f = 90.0;
        }

#endif

        break;
    }

    log_d(" angle_x %.1f , angle_y %.1f angle_f %.1f", userData->angle_x, userData->angle_y, userData->angle_f);


}
///
// Update MVP matrix based on time
//
void Update(ESContext* esContext, float deltaTime)
{
    UserData* userData = esContext->userData;
    ESMatrix modelview;



    esMatrixLoadIdentity(&userData->perspective);
    esMatrixLoadIdentity(&userData->model);
    esMatrixLoadIdentity(&userData->view);

#ifdef ORTH
    esRotate(&userData->model, userData->angle_x, 1.0, 0.0, 0.0);
    esRotate(&userData->view, userData->angle_y, 0.0, 1.0, 0.0);
#else

    // Rotate the cube
    esMatrixLoadIdentity(&modelview);
    esRotate(&userData->model, userData->angle_y, 0.0, 1.0, 0.0);
    esRotate(&modelview, userData->angle_x, 1.0, 0.0, 0.0);
    esMatrixMultiply(&userData->model, &userData->model, &modelview);

    esTranslate(&userData->view, 0.0, 0.0, -2.0);
    esPerspective(&userData->perspective, userData->angle_f, 1.0, 1.0f, 10.0f);
#endif
}
void initVAO(ESContext* esContext) {
    UserData* userData = esContext->userData;
    GLfloat vertexPos[VERTEX_LEN * VETERX_FACE] =
    {
       -0.5f,  -0.5f,  0.0f,   0.0f, 0.0f, 1.0f,  0.0f,0.0f,// v0 ����   ��
        0.0f,   0.5f,  0.0f,   0.0f, 1.0f, 0.0f,  0.5f,1.0f,// v1 ����   �� top central
        0.5f,  -0.5f,  0.0f,   1.0f, 0.0f, 0.0f,  0.0f,1.0f,// v2 ����
        0.0f,  -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,  0.5f,0.5f,// v3 ��ǰ

    };
    // Index buffer data
    GLushort indices[VERTEX_POS_SIZE * VETERX_FACE] = {
                              0, 1, 2,
                              0, 1, 3,
                              0, 2, 3,
                              3, 1, 2, 
    };

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(2, userData->vboIds);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPos),vertexPos, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &userData->vaoId);
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);


    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
        GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void*)0);

    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
        GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
        (const void*)(VERTEX_POS_SIZE * sizeof(GLfloat)));

    glVertexAttribPointer(VERTEX_TEX_COR_INDX, VERTEX_TEX_COR_SIZE,
        GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
        (const void*)((VERTEX_POS_SIZE + VERTEX_COLOR_SIZE )* sizeof(GLfloat)));

    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);
    glEnableVertexAttribArray(VERTEX_TEX_COR_INDX);

    // Reset to the default VAO
   // glBindVertexArray(0);
}

GLuint loadImage(char* path) {
    int width, height, nrChannels;
    GLuint texId = 0;
    GLenum format;
    stbi_set_flip_vertically_on_load(TRUE); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    switch (nrChannels) {
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        log_d("Error %s not support format", path);
        return 0;
    }

    if (data)
    {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //  glGenerateMipmap(GL_TEXTURE_2D);
        log_d("%s  width=%d height=%d nrChannels %d ", path, width, height, nrChannels);

    }
    else
    {
        log_d("Failed to load texture");
    }
    stbi_image_free(data);
    return texId;
}///
// Initialize the program object
//
int Init(ESContext* esContext)
{
    UserData* userData = esContext->userData;
    const char vShaderStr[] ="Hello_Triangle.vs";
   const char fShaderStr[] ="Hello_Triangle.fs";
   
    GLuint programObject;

    // Create the program object
    programObject = esLoadProgram(vShaderStr, fShaderStr);

    if (programObject == 0)
    {
        return GL_FALSE; 
    }
    userData->programObject = programObject;

    userData->scaleLoc = glGetUniformLocation(userData->programObject, "u_scale");

    userData->angle_x = userData->angle_y = userData->angle_z = userData->angle = 0.0f;
    userData->scale = 2.0f;
    userData->angle_f = 60.0f;
    userData->z_pos = -2.0f;
    glDepthRangef(-1.0, 1.0);
    // glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
#ifdef VAO
    initVAO(esContext);
#endif
    Update(esContext, 1);
    glEnable(GL_DEPTH_TEST);

    // load and create a texture 
 // -------------------------
    userData->texture1 = loadImage("gold_skin.png");

    return GL_TRUE;
}

///
// Draw a triangle using the program object created in Init()
//
void Draw(ESContext* esContext)
{
    UserData* userData = esContext->userData;

    GLfloat color[3 * 12] = { 1.0f, 0.0f, 0.0f,  // v1  R
                              0.0f, 1.0f, 0.0f,  // v2  G
                              0.0f, 0.0f, 1.0f,  // v3  B

                             0.5f, 0.5f, 0.5f,   // v0  W
                             0.0f, 1.0f, 0.0f,   // v1  G
                             0.0f, 0.0f, 1.0f,   // v2  B

                             0.5f, 0.5f, 0.5f,   // v0  W
                             0.0f, 0.0f, 1.0f,   // v2  B
                             0.0f, 0.0f, 1.0f,   // v3  B

                             0.5f, 0.5f, 0.5f,   // v0  W
                             0.0f, 1.0f, 0.0f,   // v1  G
                             0.0f, 0.0f, 1.0f,   // v3  B
    };
    // 3 vertices, with (x,y,z) per-vertex
    GLfloat vertexPos[3 * 12] =
    {
        0.5f,  -0.5f,  0.0f,// v0 ��
       0.0f,   0.5f,  0.0f, // v0 ��
       -0.5f, -0.5f,  0.0f, // v0 ��


       -0.5f, -0.5f,  0.0f, // v1 ��
       0.0f,   0.5f,  0.0f,  // v1 ��
        0.0f,  -0.5f,  0.5f,  // v1

       0.0f,   0.5f,  0.0f, // v2  ��
       0.5f, -0.5f,  0.0f, // v2
       0.0f,  -0.5f,  0.5f,  // v2

        -0.5f, -0.5f,  0.0f,  // v3  ��
       0.0f,  -0.5f,  0.5f,  // v3
       0.5f,  -0.5f,  0.0f,  // v3
    };
    GLfloat texturePos[2 * 12] = {
        1.0,0.0f ,//top central
        0.5f,1.0f,//left bottom
        0.0f,0.0f,//right bottom

        0.0,0.0f ,//top central
        0.0f,0.5f,//left bottom
        0.5f,0.0f,//right bottom

        0.0f,0.5f ,//top central
        0.0f,0.0f,//left bottom
        0.5f,0.0f,//right bottom

        0.5,1.0f ,//top central
        0.0f,0.0f,//left bottom
        1.0f,0.0f,//right bottom
    };



    glViewport(0, 0, esContext->width, esContext->height);
    // glFrontFace(GL_CCW);
    //  glCullFace(GL_FRONT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniform1i(glGetUniformLocation(userData->programObject, "texture1"), 0);


    glUseProgram(userData->programObject);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->texture1);

#ifdef VAO
    // Bind the VAO
    glBindVertexArray(userData->vaoId);
   // glFrontFace(GL_CW);
   // glCullFace(GL_FRONT);
    //glEnable(GL_CULL_FACE);

#else
    glEnable(GL_CULL_FACE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertexPos);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, color);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, texturePos);
    glEnableVertexAttribArray(2);
    //glVertexAttrib4fv ( 1, color );
#endif
       // Load the MVP matrix
    glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "model"), 1, GL_FALSE, (GLfloat*)&userData->model.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "view"), 1, GL_FALSE, (GLfloat*)&userData->view.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "perspective"), 1, GL_FALSE, (GLfloat*)&userData->perspective.m[0][0]);

    glUniform1f(userData->scaleLoc, userData->scale);
    //use texture



#ifdef VAO
    glDrawElements(GL_TRIANGLES, 3* VETERX_FACE, GL_UNSIGNED_SHORT, (const void*)0);
   // glBindVertexArray(0);
#else
    glDrawArrays(GL_TRIANGLES, 0, 12);
#endif
    //glDisableVertexAttribArray ( 0 );
    //glDisableVertexAttribArray(1);
}

void Shutdown(ESContext* esContext)
{
    UserData* userData = esContext->userData;

    glDeleteProgram(userData->programObject);
}

int esMain(ESContext* esContext)
{
    esContext->userData = malloc(sizeof(UserData));

    esCreateWindow(esContext, "Triangle", 720, 720, ES_WINDOW_RGB | ES_WINDOW_DEPTH | ES_WINDOW_ALPHA);

    if (!Init(esContext))
    {
        return GL_FALSE;
    }

    esRegisterShutdownFunc(esContext, Shutdown);
    esRegisterUpdateFunc(esContext, Update);

    esRegisterDrawFunc(esContext, Draw);
    esRegisterKeyFunc(esContext, onKeyPressed);

    return GL_TRUE;
}
#endif