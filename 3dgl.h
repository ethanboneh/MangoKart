#ifndef _GL3D_H
#define _GL3D_H

#include <gl.h>

/*
 * Type representing a 3-dimensional vector
 * @param x: the x component of the vector
 * @param y: the y component of the vector
 * @param z: the z component of the vector
 */
typedef struct
{
    float x, y, z;
} Vec3;

/*
 * Type representing a 2-dimensional vector
 * @param x: the x component of the vector
 * @param y: the y component of the vector
 */
typedef struct
{
    float x, y;
} Vec2;

/*
 * Type representing an edge
 * @param i: the index of the first vertex of the edge
 * @param j: the index of the second vertex of the edge
 */
typedef struct
{
    int i, j;
} edge;

/*
 * Type representing a face
 * @param a: the index of the first vertex of the face
 * @param b: the index of the second vertex of the face
 * @param c: the index of the third vertex of the face
 */
typedef struct
{
    int a, b, c;
    int flipped;
} face;

typedef enum
{
    DRAWABLE_FACE = 1 << 0,
    DRAWABLE_EDGE = 1 << 1,
    DRAWABLE_VERTEX = 1 << 2,
} drawable_item_t;

/*
 * Generalized wrapper type representing a drawable element (edge or face) for sorting
 * @param average_z: the average_z value of the drawable element
 * @param type: whether the drawable element is a face or an edge
 * @param item: a pointer to the drawable element itself
 */
typedef struct
{
    float average_z;
    drawable_item_t type;
    void *item;
} drawable;

typedef struct
{
    Vec3 vertices[30];
    edge edges[30];
    face faces[30];

    int num_vertices, num_faces, num_edges;
    color_t color;

    Vec3 translation;
    float scale;
} obj;

/*
 * Initialize the 3D graphics library
 *
 * @param screenW: the width of the screen
 * @param screenH: the height of the screen
 * @param eye: the initial position of the camera
 * @param center: the initial position the camera is looking at
 * @param lighting: the initial position of the light source
 */
void gl3d_init(float screenW, float screenH, Vec3 eye, Vec3 center, Vec3 lighting);

void gl3d_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color_t c);

/*
 * Draw a polygon with vertices (x1, y1), (x2, y2), (x3, y3), (x4, y4) with color `c`
 *
 * @param x1, y1: the first vertex of the polygon
 * @param x2, y2: the second vertex of the polygon
 * @param x3, y3: the third vertex of the polygon
 * @param x4, y4: the fourth vertex of the polygon
 * @param c: the color of the polygon
 */
void gl_draw_polygon(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, color_t c);

/*
 * Draw a 3D object
 *
 * @param Object: the object to draw
 */
void gl3d_draw_object(obj Object);

/*
 * Create a 3D object with vertices, edges, and faces
 *
 * @param vertices: the vertices of the object
 * @param edges: the edges of the object
 * @param faces: the faces of the object
 * @param num_vertices: the number of vertices
 * @param num_edges: the number of edges
 * @param num_faces: the number of faces
 * @param translation: translation vector for the object
 * @param scale: scale factor
 * @param color: the color of the object
 *
 * @return the object itself
 */
obj gl3d_create_object(Vec3 vertices[], edge edges[], face faces[], int num_vertices, int num_edges, int num_faces, Vec3 translation, float scale, color_t color);

/*
 * Clear the screen with color `c`
 *
 * @param c: the color to clear the screen with
 */
void gl3d_clear(color_t c);

/*
 * Draw a cube with center at `center` and width `width` with color `c`
 *
 * @param center: the center of the cube
 * @param width: the width of the cube
 * @param c: the color of the cube
 */
void gl3d_draw_cube(Vec3 center, float width, color_t c);

/*
 * Draw axes with length `length`
 *
 * @param length: the length of the axes
 */
void gl3d_draw_axes(float length);

/*
 * Move the camera to the position `eye` and look at the position `center`
 *
 * @param eye: the position of the camera
 * @param center: the position to look at
 */
void gl3d_move_camera(Vec3 eye, Vec3 center);

/*
 * Adjust the lighting of the scene
 *
 * @param lighting: the lighting to adjust
 */
void gl3d_adjust_lighting(Vec3 lighting);

/*
 * Draw all the objects in `objects` in the correct order
 *
 * @param objects: the objects to draw
 * @param num_objects: the number of objects
 */
void gl3d_draw_objects(obj objects[], int num_objects);

/*
 * Sort the objects in `Objects` in order of their average z value
 *
 * @param Objects: the objects to sort
 * @param low: the lower bound of the array
 * @param high: the upper bound of the array
 */
void gl3d_sort_objects(obj Objects[], int low, int high);

#endif
