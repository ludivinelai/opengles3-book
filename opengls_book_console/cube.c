/****************************************************************
file:         hello_Triangle.c
description:  the source file opengles draw triangle
date:         2022/05/018
author        hufengkai
****************************************************************/
#include "esUtil.h"
#include "log.h"
#include "config.h"
#include "obj_loader.h"
#ifdef CUBE
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
//#define ORTH
#define VAO

//#define HELLOE_TRIANGLE



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
    unsigned int diffuseMap;
    unsigned int normalMap;
    unsigned int specularMap;

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
    case 'o':
        userData->angle_x = userData->angle_y = userData->angle_z = userData->angle = 0.0f;
        userData->scale = 1.0f;
        userData->angle_f = 60.0f;
        log_d("onKeyPress %C reset  ", key);

    }

    log_d(" angle_x %.1f , angle_y %.1f angle_f %.1f", userData->angle_x, userData->angle_y, userData->angle_f);


}
///
// Update MVP matrix based on time
//
void Update(ESContext* esContext, float deltaTime)
{
    UserData* userData = esContext->userData;



#ifdef ORTH
    esRotate(&userData->model, userData->angle_x, 1.0, 0.0, 0.0);
    esRotate(&userData->view, userData->angle_y, 0.0, 1.0, 0.0);
#else
    ESMatrix modelview;
    esMatrixLoadIdentity(&modelview);
    esMatrixLoadIdentity(&userData->perspective);
    esMatrixLoadIdentity(&userData->model);
    esMatrixLoadIdentity(&userData->view);
    esRotate(&userData->model, userData->angle_y, 0.0, 1.0, 0.0);
    esRotate(&modelview, userData->angle_x, 1.0, 0.0, 0.0);
    esMatrixMultiply(&userData->model, &userData->model, &modelview);
    esMatrixLookAt(&userData->view, 0.0f, 0.0f, 5.0f, .0f, .0f, .0f, 0.0f, 1.0f, 0.0f);
   // esTranslate(&userData->view, 0.0, 0.0, -2.0);
    esPerspective(&userData->perspective, userData->angle_f, 1.0, 1.0f, 100.0f);
#endif
}

void tangent(GLfloat * vertices, int size) {
    GLfloat * v1, * v2 ,* v3;
    GLfloat * t1, * t2, * t3;
    GLfloat* tangent1, * tangent2, * tangent3;
    GLfloat edge1_x, edge1_y, edge1_z;
    GLfloat edge2_x, edge2_y, edge2_z;

    GLfloat deltaUV1_x, deltaUV1_y;
    GLfloat deltaUV2_x, deltaUV2_y;
    GLfloat f;
    const int offset_y = 1;
    const int offset_z = 2;
    const int line_size = 11;
    v1 = vertices;
    t1 = v1 + 3;

    //每次处理三行数据
    for (int i = 0;i < size;i += line_size*3) {
        v1 = vertices + i;
        v2 = v1 + line_size;
        v3 = v2 + line_size;

        t1 = v1 + 3;
        t2 = t1 + line_size;
        t3 = t2 + line_size;
        
        tangent1 = v1 + 8;
        tangent2 = tangent1 + line_size;
        tangent3 = tangent2 + line_size;


        edge1_x = *v2 - *v1;
        edge1_y = *(v2 + offset_y) - *(v1 + offset_y);
        edge1_z = *(v2 + offset_z) - *(v1 + offset_z);

        edge2_x = *v3 - *v1;
        edge2_y = *(v3 + offset_y) - *(v1 + offset_y);
        edge2_z = *(v3 + offset_z) - *(v1 + offset_z);

        deltaUV1_x = *t2 - *t1;
        deltaUV1_y = *(t2 + offset_y) - *(t1 + offset_y);
        deltaUV2_x = *t3 - *t1;
        deltaUV2_y = *(t3 + offset_y) - *(t1 + offset_y);

        f = 1.0f / (deltaUV1_x * deltaUV2_y - deltaUV2_x * deltaUV1_y);

        *tangent1            = f * (deltaUV2_y * edge1_x - deltaUV1_y * edge2_x);
        *(tangent1+offset_y) = f * (deltaUV2_y * edge1_y - deltaUV1_y * edge2_y);
        *(tangent1+ offset_z) = f * (deltaUV2_y * edge1_z - deltaUV1_y * edge2_z);

        *tangent3=*tangent2 = *tangent1;
        *(tangent3 + offset_y) = *(tangent2 + offset_y) = *(tangent1 + offset_y);
        *(tangent3 + offset_z) = *(tangent2 + offset_z) = *(tangent1 + offset_z);

    }

}
int g_draw_count;
void initVAO(ESContext* esContext) {
    UserData* userData = esContext->userData;
    GLfloat vertices[] =
    { 
      //   position         | texture  |      normal   |     tangent              
     //  x   ,    y ,   z     s   ,   t    x   ,  y   ,  z  , x   , y   , z
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f,

    };
    float* v;
    int size=0;
    v = load_obj_mesh("backpack.obj",&size);
    unsigned int VBO, VAO_;
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO_);
    g_draw_count = size / sizeof(float);

    tangent(v, g_draw_count);

    //tangent(vertices, sizeof(vertices)/ sizeof(float));
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
   // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, size, v, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal coord attribute
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // Reset to the default VAO
   // glBindVertexArray(0);
}
GLuint LoadTexture(void* ioContext, char* fileName)
{
    int width,
        height;

    char* buffer = esLoadTGA(ioContext, fileName, &width, &height);
    GLuint texId;

    if (buffer == NULL)
    {
        esLogMessage("Error loading (%s) image.\n", fileName);
        return 0;
    }

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    free(buffer);

    return texId;
}

GLuint loadImage(char * path,int flip ) {
    int width, height, nrChannels;
    GLuint texId=0;
    GLenum format;
    if(flip){
         stbi_set_flip_vertically_on_load(TRUE); // tell stb_image.h to flip loaded texture's on the y-axis.
    }
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    switch (nrChannels) {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        case 1:
            format = GL_RED;
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
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_2D);
        log_d("%s  width=%d height=%d nrChannels %d ", path, width,height, nrChannels);

    }
    else
    {
        log_d( "Failed to load texture" );
    }
    stbi_image_free(data);
    return texId;
}
///
// Initialize the program object
//
int Init(ESContext* esContext)
{
    UserData* userData = esContext->userData;
    const char vShaderStr[] ="VertexShader.vs";
   const char fShaderStr[] ="FragmentShader.fs";
   GLfloat lightPos[] = { 1.2f, 1.0f, 2.0f };
   GLfloat viewPos[] = { 0.0f, 0.0f, 3.0f };

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
    userData->scale = 1.0f;
    userData->angle_f = 60.0f;
    userData->z_pos = -2.0f;
     glClearColor ( 0.2f, 0.2f, 0.2f, 0.2f );
#ifdef VAO
    initVAO(esContext);
#endif
    Update(esContext, 1);
    // load and create a texture 
 // -------------------------
   // unsigned int texture1;
    // texture 1
    // 
    // ---------
    glEnable(GL_DEPTH_TEST);


    //userData->diffuseMap =loadImage("brickwall.jpg");
    //userData->normalMap = loadImage("brickwall_normal.jpg");
    userData->diffuseMap = loadImage("diffuse.jpg",0);
    userData->normalMap = loadImage("normal.png",0);
    userData->specularMap = loadImage("specular.jpg", 0);

   // userData->texture1= LoadTexture(esContext->platformData, "basemap.tga");
    glUseProgram(userData->programObject);
    glUniform1i(glGetUniformLocation(userData->programObject, "diffuseMap"), 0);
    glUniform1i(glGetUniformLocation(userData->programObject, "normalMap"), 1);
    glUniform1i(glGetUniformLocation(userData->programObject, "specularMap"), 2);

   // glUniform3fv(glGetUniformLocation(userData->programObject, "lightPos"), 3,lightPos);
   // glUniform3fv(glGetUniformLocation(userData->programObject, "viewPos"), 3, viewPos);


    return GL_TRUE;
}

///
// Draw a triangle using the program object created in Init()
//
void Draw(ESContext* esContext)
{
    UserData* userData = esContext->userData;
    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   

    // Load the MVP matrix
    glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "model"), 1, GL_FALSE, (GLfloat*)&userData->model.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "view"), 1, GL_FALSE, (GLfloat*)&userData->view.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "perspective"), 1, GL_FALSE, (GLfloat*)&userData->perspective.m[0][0]);

    glUniform1f(userData->scaleLoc, userData->scale);

    //enable Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, userData->normalMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, userData->specularMap);
    glUniform1i(glGetUniformLocation(userData->programObject, "diffuseMap"), 0);
    glUniform1i(glGetUniformLocation(userData->programObject, "normalMap"), 1);
    glUniform1i(glGetUniformLocation(userData->programObject, "specularMap"), 2);


   // glUseProgram(userData->programObject);

    //use texture
    glDrawArrays(GL_TRIANGLES, 0, g_draw_count);
}

void Shutdown(ESContext* esContext)
{
    UserData* userData = esContext->userData;

    glDeleteProgram(userData->programObject);
}

int esMain(ESContext* esContext)
{
    esContext->userData = malloc(sizeof(UserData));

    esCreateWindow(esContext, "cube", 720, 720, ES_WINDOW_RGB | ES_WINDOW_DEPTH | ES_WINDOW_ALPHA);

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