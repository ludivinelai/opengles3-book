#include "obj_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"
#define V_SIZE 3
#define VT_SIZE 2
#define VN_SIZE 3
#define B_SIZE 3
#define LINE_SIZE (V_SIZE+VT_SIZE+VN_SIZE+B_SIZE)

#define F_PER_CELL 3
#define F_CELL 3
#define F_SIZE (F_CELL*F_PER_CELL)
void dump_vertices(float* v, int size, int per_line) {
    int count = size;
    int id = 0;
    log_d("dump count %d++", count / per_line);
    for (int i = 0;i < count;i += per_line) {
        printf("%5d ", id++);
        for (int j = 0;j < per_line;j++) {
            //if (v[i + j] == 0.0 && v[i + j + 1] == 0.0 && v[i + j + 2] == 0.0 && v[i + j + 3] == 0.0) {
            //    log_d("\ndump count %d--", count / per_line);
            //    return;
            //}
            printf("%.6f ", v[i + j]);
        }
        printf("\n");
    }
    log_d("dump count %d--", count / per_line);

}

void dump_face(int* f, int size) {
    int count = size*9;
    int id = 1;
    log_d("dump count %d++", count / 9);
    for (int i = 0;i < count;i += 9) {
        printf("%d ", id++);
        for (int j = 0;j < 9;j += 3) {
            printf("%d/%d/%d ", f[i + j], f[i + j + 1], f[i + j + 2]);
        }
        printf("\n");
    }
    log_d("dump count %d--", count / 9);

}

static char* loadFile(const char* path,int *len) {

    FILE* pFile = NULL;
    char* tempString = NULL;
    int length;
    char* p = NULL;
    char temp;
    pFile = fopen(path, "rb");
    if (pFile != NULL) {
        fseek(pFile, 0, SEEK_END);
        length = ftell(pFile);
        tempString = (char*)malloc(length+1);
        memset(tempString, 0, length);
        fseek(pFile, 0, SEEK_SET);
        fread(tempString, 1, length, pFile);
        fclose(pFile);
        *len = length;
        return tempString;
    }
    else {
        log_e("Fail to open file %s", path);
        exit(1);
    }
    return NULL;
}
/*
* ��ȡ triangle�ĸ������ж�\nf���ǵġ�
*/
int getFace(char* buffer,int length) {
    int count = 0;
    char* p = buffer;
    for (int i = 0;i < length-1;i++) {
        if (*(p + i) == '\n' && *(p + i + 1) == 'f') {
            count++;
            i++;
       }
    }
    return count;
}

#define BUF_SIZE 128
int getFloat(char* src, char* dst, int len) {
    int bStart = 0;
    int index = 0;
    int i = 0;
    memset(dst, 0, len);
    for (i = 0;i < BUF_SIZE;i++) {

        if (src[i] == '.' || (src[i] >= '0' && src[i] <= '9')) {
            if (!bStart) {
                bStart = 1;
            }
            dst[index++] = src[i];
        }
        else {
            if (bStart) {
                break;
            }
        }
    }
    return i;
}

int getLineFloat(char* buffer, int offset, int strip, float* out) {
    char* p = buffer;
    char* stopstring;
    for(int i=0;i<strip;i++){
        float f = strtof(p, &stopstring);
        out[offset+i] = f;
        p = stopstring;
    }
    return p - buffer;
    
}


int f_in_line(char* buffer, int offset, int* f) {
    char* p = buffer;
    char* stopstring;

    for (int i = 0;i < F_SIZE;i++) {
        int temp = strtol(p, &stopstring,10);
        f[offset + i] = temp;
        p = stopstring+1;

    }
    return p - buffer;
}


void load_data(char* buffer, int length, float* v, float* vt, float* vn,int *f,int face) {
    char* p = buffer;
    int v_count = 0,v_index=0;
    int vt_count = 0,vt_index=0;
    int vn_count = 0,vn_index=0;
    int f_count = 0,f_index=0;
    int res = 0;

    for (int i = 0;i < length - 2;i++) {
        if (*(p + i) == '\n' && *(p + i + 1) == 'v' && *(p + i + 2) == 't') {
            res = getLineFloat(p + i + 3, vt_index, VT_SIZE, vt);
             vt_index += VT_SIZE;
             vt_count++;
            i += res;
        }
        else if (*(p + i) == '\n' && *(p + i + 1) == 'v' && *(p + i + 2) == 'n') {
            res = getLineFloat(p + i + 3, vn_index, VN_SIZE, vn);
             vn_index += VN_SIZE;
             vn_count++;
            i += res;
        }
        else if (*(p + i) == '\n' && *(p + i + 1) == 'v') {
            res = getLineFloat(p + i + 2, v_index, V_SIZE, v);
             v_index += V_SIZE;
             v_count++;
            i+= res;
        }
        else if (*(p + i) == '\n' && *(p + i + 1) == 'f') {
            res = f_in_line(p + i + 2, f_index, f);
            f_index += F_SIZE;
            f_count++;
            i+= res;
        }
    }

   // dump_vertices(v,  v_index, V_SIZE);
    //dump_vertices(vt, vt_index, VT_SIZE);
    //dump_vertices(vn, vn_index, VN_SIZE);
   // dump_face(f, f_count);
    log_d("v =%d v_index %d", v_count, v_index);
    log_d("vt =%d vt_index %d", vt_count, vt_index);
    log_d("vn =%d vn_index = %d ", vn_count, vn_index);
    log_d("f_count  =%d %d", f_count,face);


}
float* load_obj_mesh(char* path,int *size) {
    log_i("start load  obj");
    int length;
    char* obj = loadFile(path,&length);
    int face = getFace(obj, length);
    int s_size = 0;
    s_size = sizeof(float) * LINE_SIZE * face*F_CELL;
    *size = s_size;
    log_d("size %d", LINE_SIZE * face * F_CELL);
    float* vertices = (float*)malloc(s_size);
    float* v = (float*)malloc(sizeof(float) * V_SIZE * face);
    float* vt = (float*)malloc(sizeof(float) * VT_SIZE * face);
    float* vn = (float*)malloc(sizeof(float) * VN_SIZE * face);
    int* f= (int*)malloc(sizeof(int) * face * F_SIZE);
    memset(vertices, 0, s_size);
    memset(f, 0, sizeof(float) *  F_SIZE * face);
    memset(v, 0, sizeof(float) * V_SIZE * face);
    memset(vt, 0, sizeof(float) * VT_SIZE * face);
    memset(vn, 0, sizeof(float) * VN_SIZE * face);

    load_data(obj, length,v, vt, vn, f,face);
    float* p_vertces = vertices;

    for (int i = 0;i < face;i++) {

        for (int j = 0;j < F_CELL;j++) {
            int x_index ,f_index,v1;
            float f1, f2, f3;
            int index;
            f_index = i * F_SIZE + F_PER_CELL * j;
            v1 = f[f_index]-1;
            x_index = v1 * V_SIZE;
            f1 = v[x_index];
            f2 = v[x_index + 1];
            f3 = v[x_index + 2];
            index = (i* F_CELL + j) * LINE_SIZE;
           
            p_vertces[index] = f1;
            p_vertces[index + 1] = f2;
            p_vertces[index + 2] = f3;

            v1 = f[f_index + 1]-1;
            x_index = v1 * VT_SIZE;
            f1 = vt[x_index];
            f2 = vt[x_index + 1];
            p_vertces[index+3] = f1;
            p_vertces[index+4] = f2;


            v1 = f[f_index + 2]-1;
            x_index = v1 * VN_SIZE;
            f1 = vn[x_index];
            f2 = vn[x_index + 1];
            f3 = vn[x_index + 2];
            p_vertces[index+5] = f1;
            p_vertces[index+6] = f2;
            p_vertces[index+7] = f3;
            //printf("%d ", index);
            //for (int k = index;k < index + 9;k++) {
            //    printf("%.5f ", vertices[k]);
            //}
            //printf("\n");
        }


    }
  //  dump_vertices(vertices, LINE_SIZE * face, LINE_SIZE);
    free(v);
    free(vt);
    free(vn);
    free(f);
    log_i("finish load  obj");

    return vertices;

}
/* float* getVertices(int id) {

}
void load_mtl(char* path) {

}
char* getDiffuse(int id) {

}
char* getNormal(int id) {

}
char* getSpecular(int id) {

} */
