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

    obj Objects[10];

    float viewMatrix[16];
} module;

/*
 * Returns the square root of a number
 * @param n the number to find the square root of
 */
static double sqrt(double x)
{
    // keep floating point precision
    double s = 1;
    while (x > 100)
    {
        x /= 100;
        s *= 10;
    }
    int n = 15;
    double res = x / 2;
    while (n--)
    {
        res = (res + x / res) / 2;
    }
    return s * res;
}

// Basic vector operations
static Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static Vec3 vec3_add(Vec3 a, Vec3 b)
{
    return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static Vec3 vec3_multi_scalar(Vec3 vec, float scalar)
{
    return (Vec3){vec.x * scalar, vec.y * scalar, vec.z * scalar};
}

static Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

static Vec3 vec3_normalize(Vec3 v)
{
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (Vec3){v.x / len, v.y / len, v.z / len};
}

static void generate_look_at_matrix(Vec3 eye, Vec3 center, Vec3 up, float *matrix)
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

static Vec3 transform_point(float *matrix, Vec3 point)
{
    Vec3 result;
    result.x = matrix[0] * point.x + matrix[4] * point.y + matrix[8] * point.z + matrix[12];
    result.y = matrix[1] * point.x + matrix[5] * point.y + matrix[9] * point.z + matrix[13];
    result.z = matrix[2] * point.x + matrix[6] * point.y + matrix[10] * point.z + matrix[14];
    return result;
}

// culling
static int outOfBounds(Vec2 point)
{
    return (point.x > module.screenW || point.x < 0 || point.y > module.screenH || point.y < 0);
}

static Vec2 project_point(Vec3 point)
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

static Vec2 calculate_point(Vec3 point)
{
    Vec3 viewPoint = transform_point(module.viewMatrix, point);
    Vec2 screenPoint = project_point(viewPoint);

    return screenPoint;
}

static int numObj = 0;

/* =============== OBJECT-LEVEL METHODS START HERE ======================== */

// initalize objects and add to the object buffer
obj gl3d_create_object(Vec3 vertices[], edge edges[], face faces[], int num_vertices, int num_edges, int num_faces, Vec3 translation, float scale, color_t color)
{
    obj object1;

    object1.translation = translation;
    object1.scale = scale;

    object1.num_vertices = num_vertices;
    object1.num_edges = num_edges;
    object1.num_faces = num_faces;

    for (int i = 0; i < object1.num_vertices; i++)
    {
        object1.vertices[i] = vertices[i];
    }

    for (int i = 0; i < object1.num_edges; i++)
    {
        object1.edges[i] = edges[i];
    }

    for (int i = 0; i < object1.num_faces; i++)
    {
        object1.faces[i] = faces[i];
    }

    object1.color = color;

    return object1;
}

void gl3d_draw_face(Vec2 twoPts[], int numPts, color_t color)
{
    Vec2 mainPoint = twoPts[0];
    Vec2 cache = twoPts[1];

    for (int i = 2; i < numPts; i++)
    {
        Vec2 thirdPoint = twoPts[i];
        gl3d_draw_triangle(mainPoint.x, mainPoint.y, cache.x, cache.y, thirdPoint.x, thirdPoint.y, color);
        cache = thirdPoint;
    }
}

// projects, culls, then draws face
void gl3d_draw_object(obj Object)
{
    int num_vertices = Object.num_vertices;
    Vec2 projected_points[num_vertices];

    // VERTICES
    for (int i = 0; i < num_vertices; i++)
    {
        Vec2 screen_point = calculate_point(vec3_add(vec3_multi_scalar(Object.vertices[i], Object.scale), Object.translation));
        projected_points[i] = screen_point;
        // if (!outOfBounds(screen_point))
        // {
        //     gl_draw_pixel(screen_point.x, screen_point.y, Object.color);
        //     gl_draw_char(screen_point.x, screen_point.y, '0' + i, GL_BLACK); // for debugging
        // }
    }

    // FACES
    for (int i = 0; i < Object.num_faces; i++)
    {
        Vec2 face_points[3];
        face_points[0] = projected_points[Object.faces[i].a];
        face_points[1] = projected_points[Object.faces[i].b];
        face_points[2] = projected_points[Object.faces[i].c];
        gl3d_draw_face(face_points, 3, Object.color);
    }

    // EDGES
    for (int i = 0; i < Object.num_edges; i++)
    {
        Vec2 screen_point_1 = projected_points[Object.edges[i].i];
        Vec2 screen_point_2 = projected_points[Object.edges[i].j];
        if (!outOfBounds(screen_point_1) || !outOfBounds(screen_point_2))
            gl_draw_line(screen_point_1.x, screen_point_1.y, screen_point_2.x, screen_point_2.y, GL_BLACK);
    }
}

static float find_central_z(obj o)
{
    float netZ = 0;
    for (int i = 0; i < o.num_vertices; i++)
    {
        netZ += transform_point(module.viewMatrix, vec3_add(vec3_multi_scalar(o.vertices[i], o.scale), o.translation)).z;
    }
    return netZ / o.num_vertices;
}

void gl3d_draw_objects(obj objects[], int num_objects)
{
    gl3d_sort_objects(objects, 0, num_objects - 1);
    for (int i = 0; i < num_objects; i++)
    {
        gl3d_draw_object(objects[i]);
    }
}

// sorting:

static void swap_objects(obj *ob1, obj *ob2)
{
    obj temp;
    temp = *ob1;
    *ob1 = *ob2;
    *ob2 = temp;
}

static int quicksort_partition(obj Objects[], int low, int high)
{
    float pivot = find_central_z(Objects[high]); // Pivot
    int i = (low - 1);                           // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than or equal to pivot
        if (find_central_z(Objects[j]) <= pivot)
        {
            i++; // Increment index of smaller element
            swap_objects(&Objects[i], &Objects[j]);
        }
    }
    swap_objects(&Objects[i + 1], &Objects[high]);
    return (i + 1);
}

void gl3d_sort_objects(obj Objects[], int low, int high)
{
    if (low < high)
    {
        int pivot = quicksort_partition(Objects, low, high);
        gl3d_sort_objects(Objects, low, pivot - 1);
        gl3d_sort_objects(Objects, pivot + 1, high);
    }
}

/* =============== EXTERNAL DRAWING METHODS START HERE ======================== */

void gl3d_init(float screenW, float screenH, Vec3 eye, Vec3 center)
{
    module.screenW = screenW;
    module.screenH = screenH;

    module.eye = eye;
    module.center = center;
    module.up = (Vec3){0.0, 1.0, 0.0};

    generate_look_at_matrix(module.eye, module.center, module.up, module.viewMatrix);
}

void gl3d_clear(color_t c)
{
    gl_clear(c);
};

void gl3d_move_camera(Vec3 eye, Vec3 center)
{
    module.eye = eye;
    module.center = center;

    generate_look_at_matrix(module.eye, module.center, module.up, module.viewMatrix);
}

void gl3d_draw_cube(Vec3 center, float width, color_t c)
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

    Vec2 screenPoint = calculate_point(point);
    Vec2 screenPoint2 = calculate_point(point2);
    Vec2 screenPoint3 = calculate_point(point3);
    Vec2 screenPoint4 = calculate_point(point4);
    Vec2 screenPoint5 = calculate_point(point5);
    Vec2 screenPoint6 = calculate_point(point6);
    Vec2 screenPoint7 = calculate_point(point7);
    Vec2 screenPoint8 = calculate_point(point8);

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

void gl3d_draw_axes(float length)
{
    Vec3 origin = (Vec3){0, 0, 0};
    Vec3 x = (Vec3){length, 0, 0};
    Vec3 y = (Vec3){0, length, 0};
    Vec3 z = (Vec3){0, 0, length};

    Vec2 origin_screen = calculate_point(origin);
    Vec2 x_screen = calculate_point(x);
    Vec2 y_screen = calculate_point(y);
    Vec2 z_screen = calculate_point(z);

    gl_draw_line(origin_screen.x, origin_screen.y, x_screen.x, x_screen.y, GL_RED);
    gl_draw_line(origin_screen.x, origin_screen.y, y_screen.x, y_screen.y, GL_GREEN);
    gl_draw_line(origin_screen.x, origin_screen.y, z_screen.x, z_screen.y, GL_BLUE);

    int originsize = 2;
    gl_draw_string(20, 20, "Axes: X is red, Y is green, Z is black", GL_BLACK);
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
