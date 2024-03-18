#include "assert.h"
#include "console.h"
#include "fb.h"
#include "gl.h"
#include "3dgl.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "uart.h"
#include "accel_driver.h"
#include "i2c.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

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

    float pPoint[] = {30, 100, 60, 1};

    gl_clear(GL_GREEN);

    gl_draw_pixel(pPoint[0], pPoint[1], GL_PURPLE);

    // float *nPoint = projectPoint(pPoint);

    // printf("%d, ", (int)(100 * nPoint[0]));
    // printf("%d", (int)(100 * nPoint[1]));

    // gl_draw_pixel(nPoint[0] + 1, nPoint[1] + 1, GL_BLUE);

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

void test_draw_object()
{
    const int WIDTH = SCREEN_WIDTH;
    const int HEIGHT = SCREEN_HEIGHT;

    Vec3 eye = {350.0, 150.0, 500.0};
    Vec3 center = {1.0, 1.0, 50.0};

    int frame_delay = 0;  // ms
    int load_time = 5000; // ms

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl3d_init(WIDTH, HEIGHT, eye, center);

    int lastFrameTime = 10;

    Vec3 vertices[] = {
        (Vec3){50, 50, 50},
        (Vec3){100, 50, 50},
        (Vec3){100, 100, 50},
        (Vec3){50, 100, 50},
        (Vec3){50, 50, 100},
        (Vec3){100, 50, 100},
        (Vec3){100, 100, 100},
        (Vec3){50, 100, 100},
    };
    edge edges[] = {
        (edge){0, 5},
        (edge){5, 1},
        (edge){1, 0},
        (edge){1, 2},
        (edge){2, 5},
        (edge){2, 0},
    };

    face faces[] = {
        (face){2, 1, 5},
        (face){0, 1, 5},
    };

    Vec3 translation = (Vec3){100, 0, 0};
    float scale = 2;

    int num_vertices = sizeof(vertices) / sizeof(Vec3);
    int num_edges = sizeof(edges) / sizeof(edge);
    int num_faces = sizeof(faces) / sizeof(face);

    obj object1 = gl3d_create_object(vertices, edges, faces, num_vertices, num_edges, num_faces, translation, scale, GL_ORANGE);

    gl_clear(gl_color(0x25, 0x59, 0x57));
    gl_draw_string(320, 280, "Loading objects...", GL_WHITE);
    gl_swap_buffer();

int runningTotal = 0;
int startTime = timer_get_ticks();

    for (int i = 0; i < 500; i++)
    {
        
        int fps = 1000000 / lastFrameTime;
        // printf("FPS: %d\n", fps);
        gl3d_clear(GL_WHITE);

        short x_a, y_a, z_a; // Accelerometer data
        short x_g, y_g, z_g;
        mpu_read_accelerometer(&x_a, &y_a, &z_a, &x_g, &y_g, &z_g);

        printf("Gyro X: X = %d\n", x_g);

        lastFrameTime = ((timer_get_ticks() - startTime) / (100 * TICKS_PER_USEC));
        gl3d_remote_camera(x_g, lastFrameTime);
        gl3d_move_camera_forward();
        
        startTime = timer_get_ticks();
        
        // gl3d_move_camera_forward();
        // gl3d_move_camera((Vec3){3000.0, 150.0, 500.0}, center);

        gl3d_draw_axes(50);
        gl3d_draw_object(object1);

        gl_swap_buffer();

        // printf("Frame Time: %d\n", lastFrameTime);
        // pause("wait");
    }

    pause("drawn object... [CLICK ANY KEY TO CONTINUE]");
}

void main(void)
{
    uart_init();
    timer_init();

    i2c_init();
    mpu_init();

    printf("WE ONNNN\n");


    // test_gl_polygons();

    test_draw_object();
}
