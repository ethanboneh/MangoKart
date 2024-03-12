#ifndef _MY_MODULE_H
#define _MY_MODULE_H

#include <gl.h>

typedef struct
{
    float x, y, z;
} Vec3;

typedef struct
{
    float x, y;
} Vec2;

void say_hello(const char *name);

void gl3d_init(float screenW, float screenH, Vec3 eye, Vec3 center);

void gl3d_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color_t c);

void gl_draw_polygon(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, color_t c);

Vec3 vec3_sub(Vec3 a, Vec3 b);

Vec3 vec3_cross(Vec3 a, Vec3 b);

float vec3_dot(Vec3 a, Vec3 b);

Vec3 vec3_normalize(Vec3 v);

void lookAt(Vec3 eye, Vec3 center, Vec3 up, float *matrix);

Vec3 transformPoint(float *matrix, Vec3 point);

Vec2 projectPoint(Vec3 point);

Vec2 calculatePoint(Vec3 point);

void draw_cube(Vec3 center, float width, color_t c);

int cullPoint(float point[]);

#endif
