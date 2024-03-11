#ifndef _MY_MODULE_H
#define _MY_MODULE_H

void say_hello(const char *name);

void gl_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color_t c);
void gl_draw_polygon(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, color_t c);

#endif