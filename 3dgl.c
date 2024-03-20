#include "3dgl.h"
#include "printf.h"
#include "malloc.h"
#include <gl.h>

#define PI 3.141592653589793



// dimensions necessary for projection/culling:

static struct
{
    float screenW;
    float screenH;

    Vec3 eye;
    Vec3 center;
    Vec3 up;

    Vec3 lighting;

    obj Objects[10];
    int numObjects;

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

static float vec3_dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3 vec3_normalize(Vec3 v)
{
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (Vec3){v.x / len, v.y / len, v.z / len};
}

static Vec3 vec3_plane_normal(Vec3 p, Vec3 q, Vec3 r)
{
    Vec3 a = vec3_sub(q, p);
    Vec3 b = vec3_sub(r, p);

    Vec3 normal = vec3_cross(a, b);

    return vec3_normalize(normal);
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

/* =============== MATH STARTS HERE ======================== */

static float sin(float x);

static float pow_int(float x, int n) {
  float a = 1;
  if (n < 0) {
    x = 1 / x;
    n = -n;
  }

  while (n > 0) {
    if (n & 1) {
      a = a * x;
    }
    x = x * x;
    n = n / 2;
  }

  return a;
}

float fabs(float x) { return (x > 0 ? x : -x); }

static float cos(float x) {
  int sign = 1;
  // cos(-a) = cos(a)
  // a in [0, \infinity)
  if (x < 0) {
    x = -x;
  }
  // cos(a + 2k * PI) = cos(a)
  // a in [0, 2 * PI]
  if (x > 2 * PI) {
    x -= (int)(x / 2 / PI) * 2 * PI;
  }
  // cos(PI + a) = -cos(a)
  // a in [0, PI]
  if (x > PI) {
    x -= PI;
    sign *= -1;
  }
  // cos(PI - a) = -cos(a)
  // a in [0, PI / 2]
  if (x > PI / 2) {
    x = PI - x;
    sign *= -1;
  }
  // sin(PI / 2 - a) = cos(a)
  // a in [0, PI / 4]
  if (x > PI / 4) {
    return sign * sin(PI / 2 - x);
  }
  float approx = 1 - pow_int(x, 2) / 2 + pow_int(x, 4) / 24 -
                 pow_int(x, 6) / 720 + pow_int(x, 8) / 40320 -
                 pow_int(x, 10) / 3628800 + pow_int(x, 12) / 479001600 -
                 pow_int(x, 14) / 87178291200;
  return sign * approx;
}

static float sin(float x) {
  int sign = 1;
  // sin(-a) = -sin(a)
  // a in [0, \infinity)
  if (x < 0) {
    x = -x;
    sign *= -1;
  }
  // sin(a + 2k * PI) = sin(a)
  // a in [0, 2 * PI]
  if (x > 2 * PI) {
    x -= (int)(x / 2 / PI) * 2 * PI;
  }
  // sin(PI + a) = -sin(a)
  // a in [0, PI]
  if (x > PI) {
    x -= PI;
    sign *= -1;
  }
  // sin(PI - a) = sin(a)
  // a in [0, PI / 2]
  if (x > PI / 2) {
    x = PI - x;
  }
  // cos(PI / 2 - a) = sin(a)
  // a in [0, PI / 4]
  if (x > PI / 4) {
    return sign * cos(PI / 2 - x);
  }
  float approx = x - pow_int(x, 3) / 6 + pow_int(x, 5) / 120 -
                 pow_int(x, 7) / 5040 + pow_int(x, 9) / 362880 -
                 pow_int(x, 11) / 39916800 + pow_int(x, 13) / 6227020800;
  return sign * approx;
}

#define EPSILON 0.1
static float atan(float x) {
  float s = 1;
  while (fabs(x) > EPSILON) {
    x = (sqrt(1 + x * x) - 1) / x;
    s *= 2;
  }
  float approx = x - pow_int(x, 3) / 3 + pow_int(x, 5) / 5 - pow_int(x, 7) / 7 +
                 pow_int(x, 9) / 9 - pow_int(x, 11) / 11 + pow_int(x, 13) / 13 -
                 pow_int(x, 15) / 15 + pow_int(x, 17) / 17 -
                 pow_int(x, 19) / 19;
  return s * approx;
}

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

    module.Objects[module.numObjects] = object1;
    module.numObjects++;

    return object1;
}

void gl3d_draw_face(Vec2 a, Vec2 b, Vec2 c, color_t color)
{
    gl3d_draw_triangle(a.x, a.y, b.x, b.y, c.x, c.y, color);
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


//=============================================

static float find_central_z_drawable(drawable d, obj o)
{
    Vec3 *vertices = o.vertices;
    if (d.type == DRAWABLE_EDGE)
    {
        edge *e = (edge *)d.item;

        int i = transform_point(module.viewMatrix, vec3_add(vec3_multi_scalar(vertices[(*e).i], o.scale), o.translation)).z;
        int j = transform_point(module.viewMatrix, vec3_add(vec3_multi_scalar(vertices[(*e).j], o.scale), o.translation)).z;

        d.average_z = (i + j) / 2;
        // return (i + j) / 2;
        return i < j ? i : j;
    }
    else if (d.type == DRAWABLE_FACE)
    {
        face *f = (face *)d.item;

        int a = transform_point(module.viewMatrix, vec3_add(vec3_multi_scalar(vertices[(*f).a], o.scale), o.translation)).z;
        int b = transform_point(module.viewMatrix, vec3_add(vec3_multi_scalar(vertices[(*f).b], o.scale), o.translation)).z;
        int c = transform_point(module.viewMatrix, vec3_add(vec3_multi_scalar(vertices[(*f).c], o.scale), o.translation)).z;

        d.average_z = (a + b + c) / 3;
        // return (a + b + c) / 3;
        return a < b && a < c ? a : b < c ? b
                                          : c;
    }
    else
    {
        return *((int *)d.item);
    }
}

static void swap_drawables(drawable *ob1, drawable *ob2)
{
    drawable temp;
    temp = *ob1;
    *ob1 = *ob2;
    *ob2 = temp;
}

static int quicksort_partition_drawable(drawable items[], int low, int high, obj o)
{
    float pivot = find_central_z_drawable(items[high], o); // Pivot
    int i = (low - 1);                                     // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than or equal to pivot
        if (find_central_z_drawable(items[j], o) <= pivot)
        {
            i++; // Increment index of smaller element
            swap_drawables(&items[i], &items[j]);
        }
    }
    swap_drawables(&items[i + 1], &items[high]);
    return (i + 1);
}

static void gl3d_sort_drawables(drawable Objects[], int low, int high, obj o)
{
    if (low < high)
    {
        int pivot = quicksort_partition_drawable(Objects, low, high, o);
        gl3d_sort_drawables(Objects, low, pivot - 1, o);
        gl3d_sort_drawables(Objects, pivot + 1, high, o);
    }
}

static void generate_drawing_queue(obj Object, drawable drawing_queue[])
{
    // FACES
    for (int i = 0; i < Object.num_faces; i++)
    {
        drawable item;
        item.type = DRAWABLE_FACE;
        item.item = &(Object.faces[i]);
        drawing_queue[i] = item;
    }

    // EDGES
    for (int i = 0; i < Object.num_edges; i++)
    {
        drawable item;
        item.type = DRAWABLE_EDGE;
        item.item = &(Object.edges[i]);
        drawing_queue[i + Object.num_faces] = item;
    }
}

static color_t adjust_color(color_t original_color, float factor)
{
    int r = (original_color >> 16) & 0xFF;
    int g = (original_color >> 8) & 0xFF;
    int b = original_color & 0xFF;

    factor = (factor < -1.0) ? -1.0 : (factor > 1.0) ? 1.0
                                                     : factor;

    factor /= 2.0; // Factor is now in the range [-0.5, 0.5]

    if (factor > 0)
    {
        // Mix towards white
        r += (255.0 - r) * factor;
        g += (255.0 - g) * factor;
        b += (255.0 - b) * factor;
    }
    else
    {
        // Mix towards black
        r += r * factor; // factor is negative, so this reduces the color component
        g += g * factor;
        b += b * factor;
    }

    return gl_color(r, g, b);
}

static color_t calculate_lighting(Vec3 a, Vec3 b, Vec3 c, color_t original_color, int flipped)
{
    if (flipped)
    {
        Vec3 temp = a;
        a = c;
        c = temp;
    }
    Vec3 normal = vec3_normalize(vec3_plane_normal(a, b, c));
    Vec3 light = vec3_normalize(module.lighting);

    float light_intensity = vec3_dot(normal, light);
    // printf("Point: {%d, %d, %d} {%d, %d, %d} {%d, %d, %d}", (int)a.x, (int)a.y, (int)a.z, (int)b.x, (int)b.y, (int)b.z, (int)c.x, (int)c.y, (int)c.z);
    // printf("Normal: {%d, %d, %d} Light: {%d, %d, %d}\n", (int)normal.x, (int)normal.y, (int)normal.z, (int)(light.x * 1000), (int)(light.y * 1000), (int)(light.z * 1000));
    // printf("Light Intensity: %d\n", (int)(light_intensity * 1000));

    color_t shaded_color = adjust_color(original_color, light_intensity);

    return shaded_color;
}

void gl3d_draw_object(obj Object)
{
    drawable render_queue[Object.num_faces + Object.num_edges];

    generate_drawing_queue(Object, render_queue);

    int num_vertices = Object.num_vertices;
    Vec2 projected_points[num_vertices];

    gl3d_sort_drawables(render_queue, 0, Object.num_edges + Object.num_faces - 1, Object);

    // VERTICES
    for (int i = 0; i < num_vertices; i++)
    {
        Vec2 screen_point = calculate_point(vec3_add(vec3_multi_scalar(Object.vertices[i], Object.scale), Object.translation));
        projected_points[i] = screen_point;
        // gl_draw_pixel(screen_point.x, screen_point.y, Object.color);
        // gl_draw_char(screen_point.x, screen_point.y, '0' + i, GL_BLACK);
    }

    for (int i = 0; i < Object.num_edges + Object.num_faces; i++)
    {
        drawable d = render_queue[i];
        if (d.type == DRAWABLE_EDGE)
        {
            edge *e = (edge *)d.item;
            Vec2 screen_point_1 = projected_points[(*e).i];
            Vec2 screen_point_2 = projected_points[(*e).j];
            if (!outOfBounds(screen_point_1) || !outOfBounds(screen_point_2))
                gl_draw_line(screen_point_1.x, screen_point_1.y, screen_point_2.x, screen_point_2.y, GL_BLACK);
        }
        else if (d.type == DRAWABLE_FACE)
        {
            face *f = (face *)d.item;

            Vec3 a = Object.vertices[(*f).a];
            Vec3 b = Object.vertices[(*f).b];
            Vec3 c = Object.vertices[(*f).c];

            color_t color = calculate_lighting(a, b, c, Object.color, (*f).flipped);

            Vec2 a_projected = projected_points[(*f).a];
            Vec2 b_projected = projected_points[(*f).b];
            Vec2 c_projected = projected_points[(*f).c];
            gl3d_draw_face(a_projected, b_projected, c_projected, color);
        }
    }
}

void gl3d_draw_objects(obj objects[], int num_objects)
{
    gl3d_sort_objects(objects, 0, num_objects - 1);
    for (int i = 0; i < num_objects; i++)
    {
        gl3d_draw_object(objects[i]);
    }
}

/* =============== EXTERNAL DRAWING METHODS START HERE ======================== */

void gl3d_init(float screenW, float screenH, Vec3 eye, Vec3 center, Vec3 lighting)
{
    module.screenW = screenW;
    module.screenH = screenH;

    module.lighting = lighting;

    module.eye = eye;
    module.center = center;
    module.up = (Vec3){0.0, 1.0, 0.0};

    generate_look_at_matrix(module.eye, module.center, module.up, module.viewMatrix);
}

void gl3d_clear(color_t c)
{
    gl_clear(c);
};

static int max_pos_rotation = 43000;
static int max_neg_rotation = 53000;

void gl3d_remote_rotate_camera(short x_g, int time_interval) {
    float displacement = x_g / 3000;
    
    float angle = (x_g > 0) ? (time_interval) * displacement / (max_pos_rotation) * (PI / 4) :
                (time_interval) * displacement / (max_neg_rotation) * (PI / 4);
    
    // float magnitude = sqrt(module.center.x * module.center.x + module.center.z * module.center.z);
    Vec3 new_vector;
    new_vector.x = module.center.x - module.eye.x;
    new_vector.z = module.center.z - module.eye.z;
    new_vector.y = 0;


    float temp = new_vector.x * cos(angle) + new_vector.z * sin(angle);
    
    // printf("%d, %d\n", (int)(module.center.x), (int)(module.center.z));

    new_vector.z = - new_vector.x * sin(angle) + new_vector.z * cos(angle);
    new_vector.x = temp;

    module.center.x = module.eye.x + new_vector.x;
    module.center.z = module.eye.z + new_vector.z;

    // if(module.center.x > 0) {module.center.x -= 0.5;} else if (module.center.x < 0) {module.center.x += 0.5;}

    generate_look_at_matrix(module.eye, module.center, module.up, module.viewMatrix);
}

/*
static int gl3d_test_collision(Vec3 collision_vector) {
    for(int i = 0; i < module.numObjects; i++) {
        obj curr_object = module.Objects[i];
        
        int max_x = curr_object.vertices[0].x;
        int max_z = curr_object.vertices[0].z;

        int min_x = curr_object.vertices[0].x;
        int min_z = curr_object.vertices[0].z;

        for(int j = 1; j < curr_object.num_vertices; j++) {
            min_x = (curr_object.vertices[j].x < min_x) ? curr_object.vertices[j].x : min_x;
            min_z = (curr_object.vertices[j].z < min_z) ? curr_object.vertices[j].z : min_z;

            max_x = (curr_object.vertices[j].x > max_x) ? curr_object.vertices[j].x : max_x;
            max_z = (curr_object.vertices[j].z > max_z) ? curr_object.vertices[j].z : max_z;
        }

        if(collision_vector.x < max_x && collision_vector.x > min_x
        && collision_vector.z < max_z && collision_vector.z > min_z) {
            printf("helloooo\n");
            return 1;
        }
    }

    return 0;
}*/


void gl3d_move_camera_forward() {

    if(gl_read_pixel(SCREEN_WIDTH/2, SCREEN_HEIGHT/2) != GL_BLACK) return;

    Vec3 new_vector;
    new_vector.x = module.center.x - module.eye.x;
    new_vector.z = module.center.z - module.eye.z;
    new_vector.y = 0;

    float theta = atan(new_vector.x/new_vector.z);

    printf("%d, %d, %d\n", (int)(module.center.x), (int)(module.center.y), (int)(module.center.z));
    printf("%d, %d, %d\n", (int)(module.eye.x), (int)(module.eye.y), (int)(module.eye.z));

    module.eye.z -= cos(theta);
    module.eye.x -= sin(theta);
    module.center.z -= cos(theta);
    module.center.x -= sin(theta);

    generate_look_at_matrix(module.eye, module.center, module.up, module.viewMatrix);
}

void gl3d_move_camera(Vec3 eye, Vec3 center)
{
    module.eye = eye;
    module.center = center;

    generate_look_at_matrix(module.eye, module.center, module.up, module.viewMatrix);
}

void gl3d_adjust_lighting(Vec3 lighting)
{
    module.lighting = lighting;
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