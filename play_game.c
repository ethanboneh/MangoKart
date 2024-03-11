#include "uart.h"
#include "3dgl.h"
#include "printf.h"
#include "assert.h"

static void pause(const char *message)
{
    if (message)
        printf("\n%s\n", message);
    printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}

static void test_gl_lines_triangles(void)
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    // Double buffer mode, make sure you test single buffer too!
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    assert(gl_get_height() == HEIGHT);
    assert(gl_get_width() == WIDTH);

    gl_clear(GL_GREEN);
    // Show buffer with drawn contents
    gl_swap_buffer();
    pause("Cleared display with color green, click for lines");

    gl_clear(GL_CAYENNE);

    gl_draw_string(100, 50, "Testing Lines:", GL_AMBER);
    gl_draw_line(100, 100, 150, 300, GL_AMBER);
    gl_draw_line(150, 300, 230, 400, GL_BLUE);
    gl_draw_line(100, 100, 700, 500, GL_WHITE);
    gl_draw_line(100, 140, 700, 100, GL_BLACK);
    gl_draw_line(100, 100, 700, 100, GL_SILVER);
    gl_draw_line(100, 100, 100, 500, GL_CYAN);

    gl_swap_buffer();
    pause("Now displaying WIDTH x HEIGHT, testing lines");

    gl_clear(GL_INDIGO);

    gl_draw_string(100, 50, "Testing Triangles:", GL_CYAN);
    gl_draw_triangle(100, 100, 150, 300, 300, 150, GL_ORANGE);
    gl_draw_triangle(100, 150, 150, 300, 300, 150, GL_WHITE);
    gl_draw_triangle(300, 200, 300, 500, 450, 130, GL_YELLOW);
    gl_draw_triangle(400, 550, 600, 550, 500, 300, GL_BLUE);

    gl_swap_buffer();
    pause("Now displaying WIDTH x HEIGHT, testing triangles");

    gl_clear(gl_color(0x8a, 0xc9, 0x26));
    gl_draw_rect(0, 0, 800, 300, gl_color(0x00, 0xb4, 0xd8));

    gl_draw_rect(190, 40, 250, 35, gl_color(0xd6, 0x28, 0x28));
    gl_draw_string(200, 50, "Testing Polygons:", GL_WHITE);

    gl_draw_polygon(0, 600, 200, 300, 600, 300, 800, 600, gl_color(0x33, 0x33, 0x33));

    gl_draw_polygon(380, 600, 395, 300, 405, 300, 420, 600, GL_AMBER); // lines
    gl_draw_rect(300, 400, 200, 70, gl_color(0x33, 0x33, 0x33));

    gl_draw_polygon(186, 320, 200, 300, 186, 100, 200, 105, GL_SILVER); // pole

    gl_swap_buffer();
    pause("Now displaying WIDTH x HEIGHT, testing polygons");
}

void main(void)
{
    uart_init();
    test_gl_lines_triangles();
}
