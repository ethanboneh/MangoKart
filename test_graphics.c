/* File: test_gl_console.c
 * -----------------------
 * ***** TODO: add your file header comment here *****
 */
#include "assert.h"
#include "console.h"
#include "fb.h"
#include "gl.h"
#include "3dgl.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "uart.h"

// for testing

static void pause(const char *message)
{
    if (message)
        printf("\n%s\n", message);
    printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}

static void test_gl_polygons(void)
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
    pause("Cleared display with color green, click for polygons");

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

static void draw_road()
{
    // Vec3 topleft = (Vec3){-1000, 0, 0};
    // Vec3 topright = (Vec3){0, 0, 0};
    // Vec3 bottomleft = (Vec3){0, 0, 0};
    // Vec3 bottomright = (Vec3){0, 0, 0};
}

static void draw_axes()
{
    Vec3 origin = (Vec3){0, 0, 0};
    Vec3 x = (Vec3){50, 0, 0};
    Vec3 y = (Vec3){0, 50, 0};
    Vec3 z = (Vec3){0, 0, 50};

    Vec2 origin_screen = calculatePoint(origin);
    Vec2 x_screen = calculatePoint(x);
    Vec2 y_screen = calculatePoint(y);
    Vec2 z_screen = calculatePoint(z);

    gl_draw_line(origin_screen.x, origin_screen.y, x_screen.x, x_screen.y, GL_RED);
    gl_draw_line(origin_screen.x, origin_screen.y, y_screen.x, y_screen.y, GL_GREEN);
    gl_draw_line(origin_screen.x, origin_screen.y, z_screen.x, z_screen.y, GL_BLUE);

    int originsize = 2;
    gl_draw_string(20, 20, "Axes: X is red, Y is green, Z is black", GL_BLACK);
    gl_draw_rect(origin_screen.x - originsize, origin_screen.y - originsize, 2 * originsize, 2 * originsize, GL_BLACK);
}

void test_cubes()
{
    const int WIDTH = 800;
    const int HEIGHT = 600;
    // Example setup
    Vec3 eye = {450.0, 150.0, 500.0};
    Vec3 center = {0.0, 0.0, 0.0};

    int frame_delay = 0;  // ms
    int load_time = 5000; // ms

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl3d_init(WIDTH, HEIGHT, eye, center);

    gl_clear(gl_color(0x25, 0x59, 0x57));
    gl_draw_string(320, 280, "Loading...", GL_WHITE);
    gl_swap_buffer();
    timer_delay_ms(load_time);

    for (int i = 0; i < 400; i++)
    {
        gl_clear(GL_WHITE);
        draw_axes();

        draw_cube((Vec3){200 - (8 * i), 400.0 - (10 * i), -600.0}, 400, GL_BLUE);
        draw_cube((Vec3){200.0 - (12 * i), -400 + (10 * i), 0.0}, 400, GL_RED);

        timer_delay_ms(frame_delay);

        gl_swap_buffer();
    }
    pause("Drawin points");
}

void main(void)
{
    timer_init();
    uart_init();
    printf("Executing main() in test_gl_console.c\n");

    test_cubes();

    test_gl_polygons();

    printf("Completed main() in test_gl_console.c\n");
}
