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

void play_game()
{
    const int WIDTH = SCREEN_WIDTH;
    const int HEIGHT = SCREEN_HEIGHT;

    Vec3 eye = {50.0, 200.0, -300.0};
    Vec3 center = {50.0, 0.0, 300.0};
    Vec3 lighting = {-100.0, 200.0, -300.0};

    int frame_delay = 0;  // ms
    int load_time = 5000; // ms

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl3d_init(WIDTH, HEIGHT, eye, center, lighting);

    int lastFrameTime = 10;

    Vec3 grass_vertices[] = {
        (Vec3){100, 0, 0},
        (Vec3){100, 0, 200},
        (Vec3){400, 0, 0},
        (Vec3){400, 0, 200},
    };

    edge grass_edges[] = {
        (edge){0, 1},
        (edge){0, 2},
        (edge){1, 3},
        (edge){2, 3},
    };

    face grass_faces[] = {
        (face){0, 1, 2, 0},
        (face){1, 2, 3, 1},
    };

    Vec3 road_vertices[] = {
        (Vec3){0, 0, 0},
        (Vec3){100, 0, 0},
        (Vec3){0, 0, 300},
        (Vec3){100, 0, 200},
        (Vec3){400, 0, 400},
        (Vec3){300, 0, 500},
        (Vec3){300, 0, 700},
        (Vec3){400, 0, 700},
    };

    edge road_edges[] = {
        (edge){0, 1},
        (edge){1, 3},
        (edge){3, 4},
        (edge){4, 7},
        (edge){7, 6},
        (edge){6, 5},
        (edge){5, 2},
        (edge){2, 0}};

    face road_faces[] = {
        (face){0, 0, 0, 0}
        // (face){0, 1, 3, 0},
        // (face){0, 2, 3, 0},
        // (face){2, 3, 5, 0},
        // (face){3, 4, 5, 0},
        // (face){4, 5, 7, 0},
        // (face){5, 6, 7, 0},
    };

    Vec3 fence_vertices[] = {
        (Vec3){0, 0, 0},
        (Vec3){100, 0, 0},
        (Vec3){0, 0, 300},
        (Vec3){100, 0, 200},
        (Vec3){400, 0, 400},
        (Vec3){300, 0, 500},
        (Vec3){300, 0, 700},
        (Vec3){400, 0, 700},

        (Vec3){0, 20, 0},
        (Vec3){100, 20, 0},
        (Vec3){0, 20, 300},
        (Vec3){100, 20, 200},
        (Vec3){400, 20, 400},
        (Vec3){300, 20, 500},
        (Vec3){300, 20, 700},
        (Vec3){400, 20, 700},
    };

    edge fence_edges[] = {
        (edge){0, 1},
        (edge){1, 3},
        (edge){3, 4},
        (edge){4, 7},
        (edge){7, 6},
        (edge){6, 5},
        (edge){5, 2},
        (edge){2, 0},

        // (edge){9, 11},
        // (edge){11, 12},
        // (edge){12, 15},
        // (edge){15, 14},
        (edge){14, 13},
        (edge){13, 10},
        (edge){10, 8},

        (edge){0, 8},
        // (edge){1, 9},
        (edge){2, 10},
        // (edge){3, 11},
        // (edge){4, 12},
        (edge){5, 13},
        (edge){6, 14},
        // (edge){7, 15},

    };

    face fence_faces[] = {
        (face){0, 0, 0, 0}};

    Vec3 building_vertices[] = {
        (Vec3){0, 0, 320},
        (Vec3){280, 0, 520},
        (Vec3){280, 0, 900},
        (Vec3){0, 0, 700},

        (Vec3){0, 100, 320},
        (Vec3){280, 100, 520},
        (Vec3){280, 100, 900},
        (Vec3){0, 100, 700},
    };

    edge building_edges[] = {
        (edge){0, 1},
        (edge){1, 2},
        (edge){2, 3},
        (edge){3, 0},

        (edge){4, 5},
        (edge){5, 6},
        (edge){6, 7},
        (edge){7, 4},

        (edge){0, 4},
        (edge){1, 5},
        (edge){2, 6},
        (edge){3, 7},
    };

    face building_faces[] = {
        (face){4, 5, 6, 0},
        (face){4, 6, 7, 0},
    };

    Vec3 pole_vertices[] = {
        (Vec3){101, 0, 201},
        (Vec3){99, 0, 199},
        (Vec3){99, 0, 201},
        (Vec3){101, 0, 199},

        (Vec3){101, 200, 201},
        (Vec3){99, 200, 199},
        (Vec3){99, 200, 201},
        (Vec3){101, 200, 199},
    };

    edge pole_edges[] = {
        (edge){0, 1},
        (edge){1, 3},
        (edge){3, 2},
        (edge){2, 0},

        (edge){4, 5},
        (edge){5, 7},
        (edge){7, 6},
        (edge){6, 4},

        (edge){0, 4},
        (edge){1, 5},
        (edge){2, 6},
        (edge){3, 7},
    };

    face pole_faces[] = {
        (face){4, 5, 6, 0},
        (face){4, 6, 7, 0},
    };

    Vec3 finish_vertices[] = {
        (Vec3){300, 130, 700},
        (Vec3){400, 130, 700},
        (Vec3){300, 100, 700},
        (Vec3){400, 100, 700},
        (Vec3){300, 0, 700},
        (Vec3){400, 0, 700},
    };

    edge finish_edges[] = {
        (edge){0, 1},
        (edge){2, 3},
        (edge){0, 2},
        (edge){1, 3},
        (edge){2, 4},
        (edge){3, 5},
    };

    face finish_faces[] = {
        (face){0, 1, 2, 0},
        (face){1, 2, 3, 1},
    };

    Vec3 booster_vertices[] = {
        (Vec3){300, 0, 600},
        (Vec3){300, 0, 620},
        (Vec3){320, 0, 600},
        (Vec3){320, 0, 620}
    };

    edge booster_edges[] = {
        (edge){0, 1},
        (edge){0, 2},
        (edge){1, 3},
        (edge){2, 3},
    };

    face booster_faces[] = {
        (face){0, 1, 2, 0},
        (face){1, 2, 3, 1},
    };

    int num_grass_vertices = sizeof(grass_vertices) / sizeof(Vec3);
    int num_grass_edges = sizeof(grass_edges) / sizeof(edge);
    int num_grass_faces = sizeof(grass_faces) / sizeof(face);

    int num_road_vertices = sizeof(road_vertices) / sizeof(Vec3);
    int num_road_edges = sizeof(road_edges) / sizeof(edge);
    int num_road_faces = sizeof(road_faces) / sizeof(face);

    int num_fence_vertices = sizeof(fence_vertices) / sizeof(Vec3);
    int num_fence_edges = sizeof(fence_edges) / sizeof(edge);
    int num_fence_faces = sizeof(fence_faces) / sizeof(face);

    int num_building_vertices = sizeof(building_vertices) / sizeof(Vec3);
    int num_building_edges = sizeof(building_edges) / sizeof(edge);
    int num_building_faces = sizeof(building_faces) / sizeof(face);

    int num_pole_vertices = sizeof(pole_vertices) / sizeof(Vec3);
    int num_pole_edges = sizeof(pole_edges) / sizeof(edge);
    int num_pole_faces = sizeof(pole_faces) / sizeof(face);

    int num_finish_vertices = sizeof(finish_vertices) / sizeof(Vec3);
    int num_finish_edges = sizeof(finish_edges) / sizeof(edge);
    int num_finish_faces = sizeof(finish_faces) / sizeof(face);

    int num_booster_vertices = sizeof(booster_vertices) / sizeof(Vec3);
    int num_booster_edges = sizeof(booster_edges) / sizeof(edge);
    int num_booster_faces = sizeof(booster_faces) / sizeof(face);


    Vec3 translation = (Vec3){0, 0, 0};
    Vec3 translation2 = (Vec3){0, 0, 0};
    Vec3 translation3 = (Vec3){0, 0, 0};
    Vec3 translation4 = (Vec3){0, 0, 0};
    Vec3 translation5 = (Vec3){0, 0, 0};
    Vec3 translation6 = (Vec3){0, 0, 0};

    float scale = 5;
    float scale2 = 5;
    float scale3 = 5;
    float scale4 = 5;
    float scale5 = 5;
    float scale6 = 5;

    obj grass = gl3d_create_object(grass_vertices, grass_edges, grass_faces, num_grass_vertices, num_grass_edges, num_grass_faces, translation, scale, GL_MOSS);
    obj road = gl3d_create_object(road_vertices, road_edges, road_faces, num_road_vertices, num_road_edges, num_road_faces, translation2, scale2, GL_SILVER);
    obj fence = gl3d_create_object(fence_vertices, fence_edges, fence_faces, num_fence_vertices, num_fence_edges, num_fence_faces, translation3, scale3, GL_CAYENNE);
    obj building = gl3d_create_object(building_vertices, building_edges, building_faces, num_building_vertices, num_building_edges, num_building_faces, translation4, scale4, GL_CAYENNE);
    obj pole = gl3d_create_object(pole_vertices, pole_edges, pole_faces, num_pole_vertices, num_pole_edges, num_pole_faces, translation5, scale5, GL_BLUE);
    obj finish = gl3d_create_object(finish_vertices, finish_edges, finish_faces, num_finish_vertices, num_finish_edges, num_finish_faces, translation6, scale6, GL_GREEN);
    obj booster = gl3d_create_object(booster_vertices, booster_edges, booster_faces, num_booster_vertices, num_booster_edges, num_booster_faces, translation6, scale6, gl_color(0xff,0xff,0));

    obj objects[] = {road, fence, building, pole, finish, booster};

    int num_objects = sizeof(objects) / sizeof(obj);

    gl_clear(gl_color(0x25, 0x59, 0x57));
    gl_draw_string(320, 280, "Loading objects...", GL_WHITE);
    gl_swap_buffer();

    int runningTotal = 0;
    int startTime = timer_get_ticks();

    for (int i = 0; i < 1000; i++)
    {

        int fps = 1000000 / lastFrameTime;
        // printf("FPS: %d\n", fps);
        gl3d_clear(gl_color(0x8e, 0xca, 0xe6));

        short x_a, y_a, z_a; // Accelerometer data
        short x_g, y_g, z_g;
        mpu_read_accelerometer(&x_a, &y_a, &z_a, &x_g, &y_g, &z_g);

        // printf("Gyro X: X = %d\n", x_g);

        lastFrameTime = ((timer_get_ticks() - startTime) / (100 * TICKS_PER_USEC));
        gl3d_remote_rotate_camera(x_g, lastFrameTime);
        gl3d_move_camera_forward(20);
        // gl3d_move_camera((Vec3){eye.x + (10 * i), eye.y, eye.z * (10 * i)}, center);

        startTime = timer_get_ticks();

        // gl3d_move_camera_forward();
        // gl3d_move_camera((Vec3){3000.0, 150.0, 500.0}, center);

        gl3d_draw_axes(50);
        gl3d_draw_objects(objects, num_objects);

        // object in the middle
        if(i == 0 || i == 1) {
            gl_draw_triangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 5, SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 - 8, SCREEN_WIDTH / 2 + 5, SCREEN_HEIGHT / 2 - 8, gl_color(0xff, 0xc0, 0xcb));
        }

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

    play_game();
}
