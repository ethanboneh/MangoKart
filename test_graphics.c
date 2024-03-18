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
    const int WIDTH = SCREEN_WIDTH;
    const int HEIGHT = SCREEN_HEIGHT;

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

void test_cubes()
{
    const int WIDTH = SCREEN_WIDTH;
    const int HEIGHT = SCREEN_HEIGHT;
    // Example setup
    Vec3 eye = {450.0, 150.0, 500.0};
    Vec3 center = {0.0, 0.0, 0.0};

    int frame_delay = 0;  // ms
    int load_time = 5000; // ms

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl3d_init(WIDTH, HEIGHT, eye, center);

    gl_clear(gl_color(0x25, 0x59, 0x57));
    gl_draw_string(280, 280, "Loading cubes...", GL_WHITE);
    gl_swap_buffer();
    timer_delay_ms(load_time);

    for (int i = 0; i < 400; i++)
    {
        gl_clear(GL_WHITE);
        gl3d_draw_axes(50);

        gl3d_draw_cube((Vec3){200 - (8 * i), 400.0 - (10 * i), -600.0}, 400, GL_BLUE);
        gl3d_draw_cube((Vec3){200.0 - (12 * i), -400 + (10 * i), 0.0}, 400, GL_RED);

        timer_delay_ms(frame_delay);

        gl_swap_buffer();
    }
    pause("Drawing moving cubes");
}

void test_cubes_camera_movement()
{

    const int WIDTH = SCREEN_WIDTH;
    const int HEIGHT = SCREEN_HEIGHT;

    Vec3 eye = {450.0, 150.0, 500.0};
    Vec3 center = {0.0, 0.0, 0.0};

    int frame_delay = 0;  // ms
    int load_time = 5000; // ms

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl3d_init(WIDTH, HEIGHT, eye, center);

    gl_clear(gl_color(0x25, 0x59, 0x57));
    gl_draw_string(240, 280, "Loading camera movement...", GL_WHITE);
    gl_swap_buffer();

    timer_delay_ms(load_time);
    int lastFrameTime = 10;

    for (int i = 0; i < 500; i++)
    {
        int startTime = timer_get_ticks();
        int fps = 1000000 / lastFrameTime;
        printf("FPS: %d\n", fps);
        gl_clear(GL_SILVER);
        gl3d_move_camera((Vec3){1000.0 + (30 * i), -200.0 + (50 * i), 1000.0 - (50 * i)}, center);
        gl3d_draw_axes(50);

        gl3d_draw_cube((Vec3){200, 400.0, -600.0}, 400, GL_BLUE);
        gl3d_draw_cube((Vec3){200.0, -400, 0.0}, 400, GL_RED);

        // timer_delay_ms(frame_delay);

        gl_swap_buffer();
        lastFrameTime = ((timer_get_ticks() - startTime) / TICKS_PER_USEC);
    }
    pause("Drawin points");
}

void test_draw_object()
{
    const int WIDTH = SCREEN_WIDTH;
    const int HEIGHT = SCREEN_HEIGHT;

    Vec3 eye = {350.0, 150.0, 300.0};
    Vec3 center = {0.0, 0.0, 0.0};

    int frame_delay = 0;  // ms
    int load_time = 5000; // ms

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl3d_init(WIDTH, HEIGHT, eye, center);

    int lastFrameTime = 10;

    Vec3 vertices[] = {
        (Vec3){0, 0, 0},
        (Vec3){100, 0, 0},
        (Vec3){100, 100, 0},
        (Vec3){0, 100, 0},
        (Vec3){0, 0, 100},
        (Vec3){100, 0, 100},
        (Vec3){100, 100, 100},
        (Vec3){0, 100, 100},
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

    // testing sorting
    Vec3 vertices2[] = {
        (Vec3){0, 0, 0},
        (Vec3){100, 0, 0},
        (Vec3){100, 100, 0},
        (Vec3){0, 100, 0},
        (Vec3){0, 0, 50},
        (Vec3){100, 0, 50},
        (Vec3){100, 100, 50},
        (Vec3){0, 100, 50},
    };

    Vec3 vertices3[] = {
        (Vec3){0, 0, 0},
        (Vec3){100, 0, 0},
        (Vec3){100, 100, 0},
        (Vec3){0, 100, 0},
        (Vec3){0, 0, 25},
        (Vec3){100, 0, 25},
        (Vec3){100, 100, 25},
        (Vec3){0, 100, 25},
    };


    obj object2 = gl3d_create_object(vertices2, edges, faces, num_vertices, num_edges, num_faces, translation, scale, GL_ORANGE);
    obj object3 = gl3d_create_object(vertices3, edges, faces, num_vertices, num_edges, num_faces, translation, scale, GL_ORANGE);

    obj Objects[] = {object3, object2, object1};
    gl3d_sort_objects(Objects, 0, 2);

    printf("\n%d", (int)Objects[0].vertices[5].z);
    printf("\n%d", (int)Objects[1].vertices[5].z);

    timer_delay_ms(load_time);

    for (int i = 0; i < 500; i++)
    {
        int startTime = timer_get_ticks();
        int fps = 1000000 / lastFrameTime;
        printf("FPS: %d\n", fps);
        gl3d_clear(GL_WHITE);
        gl3d_move_camera((Vec3){3000.0 - (10 * i), 150.0 + (2 * i), 500.0 + (2 * i)}, center);

        gl3d_draw_axes(50);
        gl3d_draw_object(object1);

        gl_swap_buffer();
        lastFrameTime = ((timer_get_ticks() - startTime) / TICKS_PER_USEC);
    }

    pause("drawn object... [CLICK ANY KEY TO CONTINUE]");
}

void main(void)
{

    timer_init();
    uart_init();
    printf("Executing main() in test_gl_console.c\n");



    short x_a, y_a, z_a; // Accelerometer data
    short x_g, y_g, z_g; // Gyroscope data

    i2c_init();
    mpu_init();

    while(1) {
        mpu_read_accelerometer(&x_a, &y_a, &z_a, &x_g, &y_g, &z_g);
        // printf("Accel: X=%d, Y=%d, Z=%d\n", x_a, y_a, z_a);
        printf("Gyro X: X = %d\n", x_g/3000);
        // printf("Gyro: X=%d, Y=%d, Z=%d\n", x_g/100, y_g/100, z_g/100);
        // printf("%d \n", x_a);
        // pause("observation");
        timer_delay_ms(100);
    }



    // test_cubes();
    // test_cubes_camera_movement();
    // test_draw_object();

    // test_gl_polygons();

    printf("Completed main() in test_gl_console.c\n");
}
