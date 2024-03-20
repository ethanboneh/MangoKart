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

    Vec3 eye = {0.0, 150.0, 800.0};
    Vec3 center = {0.0, 0.0, 0.0};
    Vec3 lighting = {-100.0, 200.0, -300.0};

    int frame_delay = 0;  // ms
    int load_time = 5000; // ms

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl3d_init(WIDTH, HEIGHT, eye, center, lighting);

    int lastFrameTime = 10;

    Vec3 quad_entrance_one[] = {
        (Vec3){50,0,500},
        (Vec3){50,0,450},
        (Vec3){200,0,500},
        (Vec3){200,0,450},

        (Vec3){50,200,500},
        (Vec3){50,200,450},
        (Vec3){200,200,500},
        (Vec3){200,200,450}
    };

    Vec3 quad_entrance_two[] = {
        (Vec3){-50,0,500},
        (Vec3){-50,0,450},
        (Vec3){-100,0,500},
        (Vec3){-100,0,450},

        (Vec3){-50,200,500},
        (Vec3){-50,200,450},
        (Vec3){-100,200,500},
        (Vec3){-100,200,450}
    };

    Vec3 quad_exit_one[] = {
        (Vec3){-100,0,500},
        (Vec3){-100,0,400},
        (Vec3){-150,0,500},
        (Vec3){-150,0,400},

        (Vec3){-100,200,500},
        (Vec3){-100,200,400},
        (Vec3){-150,200,500},
        (Vec3){-150,200,400}
    };

    Vec3 quad_exit_two[] = {
        (Vec3){-100,0,350},
        (Vec3){-100,0,150},
        (Vec3){-150,0,350},
        (Vec3){-150,0,150},

        (Vec3){-100,200,350},
        (Vec3){-100,200,150},
        (Vec3){-150,200,350},
        (Vec3){-150,200,150}
    };

    Vec3 quad_wall_one[] = {
        (Vec3){200,0,150},
        (Vec3){200,0,100},
        (Vec3){-150,0,150},
        (Vec3){-150,0,100},

        (Vec3){200,200,150},
        (Vec3){200,200,100},
        (Vec3){-150,200,150},
        (Vec3){-150,200,100}
    };

    Vec3 quad_wall_two[] = {
        (Vec3){200,0,500},
        (Vec3){200,0,100},
        (Vec3){220,0,500},
        (Vec3){220,0,100},

        (Vec3){200,200,500},
        (Vec3){200,200,100},
        (Vec3){220,200,500},
        (Vec3){220,200,100}
    };

    Vec3 building_vertices[] = {
        (Vec3){220,0,500},
        (Vec3){220,0,-800},
        (Vec3){500,0,500},
        (Vec3){500,0,-800},

        (Vec3){220,100,500},
        (Vec3){220,100,-800},
        (Vec3){500,100,500},
        (Vec3){500,100,-800},
    };



    Vec3 quad_vertices[] = {
        (Vec3){200,0,500},
        (Vec3){200,0,-800},
        (Vec3){220,0,500},
        (Vec3){220,0,-800},

        (Vec3){200,10,500},
        (Vec3){200,10,-800},
        (Vec3){220,10,500},
        (Vec3){220,10,-800},
    };

    Vec3 grass_vertices[] = {
        (Vec3){-200,0,-800},
        (Vec3){-200,0,-1000},
        (Vec3){500,0,-800},
        (Vec3){500,0,-1000},

        (Vec3){-200,10,-800},
        (Vec3){-200,10,-1000},
        (Vec3){500,10,-800},
        (Vec3){500,10,-1000},
    };

    Vec3 side_vertices[] = {
        (Vec3){-200,0,500},
        (Vec3){-200,0,-800},
        (Vec3){-220,0,500},
        (Vec3){-220,0,-800},

        (Vec3){-200,10,500},
        (Vec3){-200,10,-800},
        (Vec3){-220,10,500},
        (Vec3){-220,10,-800},
    };

    edge grass_edges[] = {
        (edge){0, 1},
        (edge){0, 2},
        (edge){0, 4},

        (edge){1, 3},
        (edge){1, 4},

        (edge){2, 3},
        (edge){2, 6},

        (edge){3, 7},
        
        (edge){4,5},
        (edge){4,6},
        (edge){5,7},
        (edge){6,7}
    };

    face grass_faces[] = {
        (face){0,1,2,0},
        (face){1,2,3,0},

        (face){0,1,5,0},
        (face){0,4,5,0},

        (face){0,2,6,0},
        (face){0,4,6,0},

        (face){2,3,7,0},
        (face){2,6,7,0},

        (face){1,3,7,0},
        (face){1,5,7,0},

        (face){4,6,7,0},
        (face){4,5,7,0},
    };

    int num_grass_vertices = sizeof(grass_vertices) / sizeof(Vec3);
    int num_grass_edges = sizeof(grass_edges) / sizeof(edge);
    int num_grass_faces = sizeof(grass_faces) / sizeof(face);


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
        (face){2, 1, 5, 0},
        (face){0, 1, 5, 0},
    };

    edge cube_edges[] = {
        (edge){1, 5},
        (edge){5, 4},
        (edge){0, 4},
        (edge){0, 1},
        (edge){2, 6},
        (edge){6, 7},
        (edge){3, 7},
        (edge){3, 2},
        (edge){5, 6},
        (edge){4, 7},
        (edge){2, 1},
        (edge){0, 3},
    };

    face cube_faces[] = {
        (face){2, 1, 5, 0},
        (face){2, 6, 5, 1},

        (face){6, 4, 5, 0},
        (face){6, 4, 7, 1},
        (face){0, 7, 4, 0},
        (face){0, 7, 3, 1},
        // (face){0, 2, 1, 0},
        // (face){0, 2, 3, 1},

        (face){6, 3, 7, 0},
        (face){2, 3, 6, 0},
    };

    Vec3 translation = (Vec3){0, 0, 0};
    Vec3 translation2 = (Vec3){-100, 0, 0};
    Vec3 translation3 = (Vec3){0, 0, 100};
    Vec3 translation4 = (Vec3){-200, 0, 200};

    float scale = 1;
    float scale2 = 1;
    float scale3 = 3;

    int num_vertices = sizeof(vertices) / sizeof(Vec3);
    int num_edges = sizeof(edges) / sizeof(edge);
    int num_faces = sizeof(faces) / sizeof(face);

    int num_cube_edges = sizeof(cube_edges) / sizeof(edge);
    int num_cube_faces = sizeof(cube_faces) / sizeof(face);

    obj grass = gl3d_create_object(grass_vertices, grass_edges, grass_faces, num_grass_vertices, num_grass_edges, num_grass_faces, translation, scale, gl_color(0xa6,0x7b,0x5b));
    obj quad = gl3d_create_object(quad_vertices, grass_edges, grass_faces, num_grass_vertices, num_grass_edges, num_grass_faces, translation, scale, gl_color(0xd2, 0xb4, 0x8c));
    obj building = gl3d_create_object(building_vertices, grass_edges, grass_faces, num_grass_vertices, num_grass_edges, num_grass_faces, translation, scale, gl_color(0xa6,0x7b,0x5b));

    /*
    obj object2 = gl3d_create_object(vertices, edges, faces, num_vertices, num_edges, num_faces, translation2, scale2, GL_MOSS);
    obj object3 = gl3d_create_object(vertices, edges, faces, num_vertices, num_edges, num_faces, translation3, scale3, GL_CYAN);
    obj object4 = gl3d_create_object(vertices, cube_edges, cube_faces, num_vertices, num_cube_edges, num_cube_faces, translation4, scale, GL_RED);
    */

    obj objects[] = {grass, quad, building};

    int num_objects = sizeof(objects) / sizeof(obj);

    gl_clear(gl_color(0x25, 0x59, 0x57));
    gl_draw_string(320, 280, "Loading objects...", GL_WHITE);
    gl_draw_rect(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 + 10, 10, 10, GL_PINK);
    gl_swap_buffer();
    gl_draw_rect(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 + 10, 10, 10, GL_PINK);

int runningTotal = 0;
int startTime = timer_get_ticks();

    for (int i = 0; i < 500; i++)
    {
        
        int fps = 1000000 / lastFrameTime;
        // printf("FPS: %d\n", fps);
        gl3d_clear(GL_BLACK);

        short x_a, y_a, z_a; // Accelerometer data
        short x_g, y_g, z_g;
        mpu_read_accelerometer(&x_a, &y_a, &z_a, &x_g, &y_g, &z_g);

        // printf("Gyro X: X = %d\n", x_g);

        lastFrameTime = ((timer_get_ticks() - startTime) / (100 * TICKS_PER_USEC));
        gl3d_remote_rotate_camera(x_g, lastFrameTime);
        gl3d_move_camera_forward();
        
        startTime = timer_get_ticks();
        
        // gl3d_move_camera_forward();
        // gl3d_move_camera((Vec3){3000.0, 150.0, 500.0}, center);

        gl3d_draw_axes(50);
        gl3d_draw_objects(objects, num_objects);

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
