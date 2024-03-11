#include "3dgl.h"
#include "printf.h"
#include <gl.h>

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

void gl_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color_t c)
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

    gl_draw_triangle(x1, y1, x2, y2, x3, y3, c);
    gl_draw_triangle(x4, y4, x2, y2, x3, y3, c);
}
