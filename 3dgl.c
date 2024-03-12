#include "3dgl.h"
#include "printf.h"
#include "malloc.h"
#include <gl.h>

// dimensions necessary for projection/culling:

static struct
{
    float screenW;
    float screenH;
    Vec3 eye;
    Vec3 center;
    Vec3 up;
    float viewMatrix[16];
} module;

static int theta;

void gl3d_init(float screenW, float screenH, Vec3 eye, Vec3 center)
{
    module.screenW = screenW;
    module.screenH = screenH;

    module.eye = eye;
    module.center = center;
    module.up = (Vec3){0.0, 1.0, 0.0};

    lookAt(module.eye, module.center, module.up, module.viewMatrix);
}

/*
 * Returns the square root of a number
 * @param n the number to find the square root of
 */
static double sqrt(double n)
{
    double lo = 1 < n ? 1 : n;
    double hi = 1 > n ? 1 : n;
    double mid;

    while (100 * lo * lo < n)
        lo *= 10;
    while (0.01 * hi * hi > n)
        hi *= 0.1;

    for (int i = 0; i < 100; i++)
    {
        mid = (lo + hi) / 2;
        if (mid * mid == n)
            return mid;
        if (mid * mid > n)
            hi = mid;
        else
            lo = mid;
    }
    return mid;
}

// Basic vector operations
Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

Vec3 vec3_normalize(Vec3 v)
{
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (Vec3){v.x / len, v.y / len, v.z / len};
}

void lookAt(Vec3 eye, Vec3 center, Vec3 up, float *matrix)
{
    Vec3 f = vec3_normalize(vec3_sub(center, eye));
    Vec3 s = vec3_normalize(vec3_cross(f, up));
    Vec3 u = vec3_cross(s, f);

    for (int i = 0; i < 16; i++)
        matrix[i] = 0;
    matrix[0] = s.x;
    matrix[4] = s.y;
    matrix[8] = s.z;
    matrix[1] = u.x;
    matrix[5] = u.y;
    matrix[9] = u.z;
    matrix[2] = -f.x;
    matrix[6] = -f.y;
    matrix[10] = -f.z;
    matrix[15] = 1;
    matrix[12] = -(s.x * eye.x + s.y * eye.y + s.z * eye.z);
    matrix[13] = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
    matrix[14] = (f.x * eye.x + f.y * eye.y + f.z * eye.z);
}

Vec3 transformPoint(float *matrix, Vec3 point)
{
    Vec3 result;
    result.x = matrix[0] * point.x + matrix[4] * point.y + matrix[8] * point.z + matrix[12];
    result.y = matrix[1] * point.x + matrix[5] * point.y + matrix[9] * point.z + matrix[13];
    result.z = matrix[2] * point.x + matrix[6] * point.y + matrix[10] * point.z + matrix[14];
    return result;
}

Vec2 projectPoint(Vec3 point)
{
    // Assuming the point is already in camera view space, apply perspective projection
    float x = point.x / point.z;
    float y = point.y / point.z;

    // Map x and y from [-nPlane, nPlane] to [0, screenW] and [0, screenH], respectively
    Vec2 newPoint;
    newPoint.x = (x + 1) * module.screenW * 0.5;
    newPoint.y = (y + 1) * module.screenH * 0.5;

    return newPoint;
}

Vec2 calculatePoint(Vec3 point)
{
    Vec3 viewPoint = transformPoint(module.viewMatrix, point);
    Vec2 screenPoint = projectPoint(viewPoint);

    return screenPoint;
}

void draw_cube(Vec3 center, float width, color_t c)
{
    Vec3 cube_center = center;
    float cube_width = width;

    float half_width = cube_width / 2.0;

    Vec3 point = (Vec3){cube_center.x + half_width, cube_center.y + half_width, cube_center.z - half_width};
    Vec3 point2 = (Vec3){cube_center.x - half_width, cube_center.y + half_width, cube_center.z - half_width};
    Vec3 point3 = (Vec3){cube_center.x + half_width, cube_center.y - half_width, cube_center.z - half_width};
    Vec3 point4 = (Vec3){cube_center.x - half_width, cube_center.y - half_width, cube_center.z - half_width};
    Vec3 point5 = (Vec3){cube_center.x + half_width, cube_center.y + half_width, cube_center.z + half_width};
    Vec3 point6 = (Vec3){cube_center.x - half_width, cube_center.y + half_width, cube_center.z + half_width};
    Vec3 point7 = (Vec3){cube_center.x + half_width, cube_center.y - half_width, cube_center.z + half_width};
    Vec3 point8 = (Vec3){cube_center.x - half_width, cube_center.y - half_width, cube_center.z + half_width};

    Vec2 screenPoint = calculatePoint(point);
    Vec2 screenPoint2 = calculatePoint(point2);
    Vec2 screenPoint3 = calculatePoint(point3);
    Vec2 screenPoint4 = calculatePoint(point4);
    Vec2 screenPoint5 = calculatePoint(point5);
    Vec2 screenPoint6 = calculatePoint(point6);
    Vec2 screenPoint7 = calculatePoint(point7);
    Vec2 screenPoint8 = calculatePoint(point8);

    gl_draw_pixel(screenPoint.x, screenPoint.y, c);
    gl_draw_pixel(screenPoint2.x, screenPoint2.y, c);
    gl_draw_pixel(screenPoint3.x, screenPoint3.y, c);
    gl_draw_pixel(screenPoint4.x, screenPoint4.y, c);
    gl_draw_pixel(screenPoint5.x, screenPoint5.y, c);
    gl_draw_pixel(screenPoint6.x, screenPoint6.y, c);
    gl_draw_pixel(screenPoint7.x, screenPoint7.y, c);
    gl_draw_pixel(screenPoint8.x, screenPoint8.y, c);

    // front face
    gl_draw_line(screenPoint.x, screenPoint.y, screenPoint2.x, screenPoint2.y, c);
    gl_draw_line(screenPoint.x, screenPoint.y, screenPoint3.x, screenPoint3.y, c);
    gl_draw_line(screenPoint2.x, screenPoint2.y, screenPoint4.x, screenPoint4.y, c);
    gl_draw_line(screenPoint3.x, screenPoint3.y, screenPoint4.x, screenPoint4.y, c);

    // connectors
    gl_draw_line(screenPoint.x, screenPoint.y, screenPoint5.x, screenPoint5.y, c);
    gl_draw_line(screenPoint2.x, screenPoint2.y, screenPoint6.x, screenPoint6.y, c);
    gl_draw_line(screenPoint3.x, screenPoint3.y, screenPoint7.x, screenPoint7.y, c);
    gl_draw_line(screenPoint4.x, screenPoint4.y, screenPoint8.x, screenPoint8.y, c);

    // back face
    gl_draw_line(screenPoint5.x, screenPoint5.y, screenPoint6.x, screenPoint6.y, c);
    gl_draw_line(screenPoint5.x, screenPoint5.y, screenPoint7.x, screenPoint7.y, c);
    gl_draw_line(screenPoint6.x, screenPoint6.y, screenPoint8.x, screenPoint8.y, c);
    gl_draw_line(screenPoint7.x, screenPoint7.y, screenPoint8.x, screenPoint8.y, c);
}

int cullPoint(float point[])
{
    // if (point[3] > module.fPlane)
    //     return 0;
    return 1;
}

/*
 * Swaps the values of two integers in place
 * @param a pointer to the first integer
 * @param b pointer to the second integer
 */
static void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void gl3d_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color_t c)
{
    // Sort the vertices in ascending order based on y-coordinate
    if (y1 > y2)
    {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }
    if (y1 > y3)
    {
        swap(&x1, &x3);
        swap(&y1, &y3);
    }
    if (y2 > y3)
    {
        swap(&x2, &x3);
        swap(&y2, &y3);
    }

    for (int y = y1; y <= y3; y++)
    {
        int minX = x1, maxX = x1;

        // Adjust minX and maxX to span the current y level
        if (y < y2)
        {
            // Scan from y1 to y2
            minX = (x1 * (y2 - y) + x2 * (y - y1)) / (y2 - y1);
        }
        else
        {
            // Scan from y2 to y3
            minX = (x2 * (y3 - y) + x3 * (y - y2)) / (y3 - y2);
        }

        if (y <= y3)
        {
            if (y == y2 && y2 == y3)
            {
                minX = x2 < x3 ? x2 : x3;
                maxX = x2 < x3 ? x3 : x2;
            }
            else
            {
                // Scan from y1 to y3 for maxX
                maxX = (x1 * (y3 - y) + x3 * (y - y1)) / (y3 - y1);
            }
        }

        // Ensure minX is less than maxX
        if (minX > maxX)
        {
            swap(&minX, &maxX);
        }

        // Draw horizontal line for the current y level
        for (int x = minX; x <= maxX; x++)
        {
            gl_draw_pixel(x, y, c);
        }
    }
}

void gl_draw_polygon(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, color_t c)
{
    // Sort the vertices in ascending order based on y-coordinate
    if (y1 > y2)
    {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }
    if (y1 > y3)
    {
        swap(&x1, &x3);
        swap(&y1, &y3);
    }
    if (y1 > y4)
    {
        swap(&x1, &x4);
        swap(&y1, &y4);
    }
    if (y2 > y3)
    {
        swap(&x2, &x3);
        swap(&y2, &y3);
    }
    if (y2 > y4)
    {
        swap(&x2, &x4);
        swap(&y2, &y4);
    }
    if (y2 > y3)
    {
        swap(&x2, &x3);
        swap(&y2, &y3);
    }

    gl3d_draw_triangle(x1, y1, x2, y2, x3, y3, c);
    gl3d_draw_triangle(x4, y4, x2, y2, x3, y3, c);
}
