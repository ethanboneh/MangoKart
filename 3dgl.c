#include "3dgl.h"
#include "printf.h"
#include "malloc.h"
#include <gl.h>

// dimensions necessary for projection/culling:
static float screenW = 600;
static float screenH = 600;
static float nPlane = 60;
static float fPlane = 1000;

// theta = (h/2)/nearPlane;
static int theta;


/* Projection Perspective Matrix:
1/((w/h)tan(theta/2))  0  0  0
0	1/tan(theta/2)    0  0
0	0		f/(f-n) -fn/(f-n)
0	0		1    0
*/

// projects and does the perspective transform of the point
float* projectPoint(float oldPoint[]) {

    // apply projection matrix
    float *newPoint = (float *)malloc(4 * sizeof(float));
    newPoint[0] = oldPoint[0] / (screenW/screenH * (screenH/2)/nPlane);
    newPoint[1] = oldPoint[1] / ((screenH/2)/nPlane);
    newPoint[2] = oldPoint[2] * fPlane/(fPlane - nPlane) + oldPoint[3] * (-fPlane * nPlane)/(fPlane - nPlane);
    newPoint[3] = oldPoint[2];

    printf("%d\n", (int)newPoint[0]);
    printf("%d\n", (int)newPoint[3]);
    // scale x and y back up to the screen
    newPoint[0] = (newPoint[0]/newPoint[3]); // + 1) * screenW/2;
    newPoint[1] = (newPoint[1]/newPoint[3]); // + 1) * screenH/2;

    return newPoint;
}

int cullPoint(float point[]) {
    if(point[3] > fPlane) return 0;
    return 1;
}

/*
static struct 3dshape {
    int (*x)[10];
    int (*y)[10];
    int (*z)[10];
    int numVertices;
} 3dshape;

static struct 2dshape {
    int (*x)[10];
    int (*y)[10];
    int numVertices;
} 2dshape;


static struct *currShape = (struct *currShape)malloc(sizeof(struct shape));
static int d = 8;
*/

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
