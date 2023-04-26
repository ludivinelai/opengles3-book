#pragma once
#ifndef __OBJ_LOADER_H__
#define __OBJ_LOADER_H__

float* load_obj_mesh(char* path,int *size);
float* getVertices(int id);
void load_mtl(char* path);
char* getDiffuse(int id);
char* getNormal(int id);
char* getSpecular(int id);

#endif