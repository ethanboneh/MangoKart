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
void gl_shade_rect(int x, int y, int w, int h, color_t c, double opacity);

static void pause(const char *message)
{
    if (message)
        printf("\n%s\n", message);
    printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}

static void test_fb(void)
{
    const int SIZE = 500;
    fb_init(SIZE, SIZE, FB_SINGLEBUFFER); // init single buffer

    assert(fb_get_width() == SIZE);
    assert(fb_get_height() == SIZE);
    assert(fb_get_depth() == 4);

    unsigned char *cptr = fb_get_draw_buffer();
    assert(cptr != NULL);
    int nbytes = fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0x99, nbytes); // fill entire framebuffer with light gray pixels
    pause("Now displaying 500 x 500 screen of light gray pixels");

    fb_init(1280, 720, FB_DOUBLEBUFFER); // init double buffer
    cptr = fb_get_draw_buffer();
    nbytes = fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0xff, nbytes); // fill one buffer with white pixels
    fb_swap_buffer();
    pause("Now displaying 1280 x 720 white pixels");

    cptr = fb_get_draw_buffer();
    memset(cptr, 0x33, nbytes); // fill other buffer with dark gray pixels
    fb_swap_buffer();
    pause("Now displaying 1280 x 720 dark gray pixels");

    for (int i = 0; i < 5; i++)
    {
        fb_swap_buffer();
        timer_delay_ms(250);
    }
}


static void test_gl(void)
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    // Double buffer mode, make sure you test single buffer too!
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    assert(gl_get_height() == HEIGHT);
    assert(gl_get_width() == WIDTH);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55)); // create purple color

    // Draw green pixel in lower right
    gl_draw_pixel(WIDTH - 10, HEIGHT - 10, GL_GREEN);
    assert(gl_read_pixel(WIDTH - 10, HEIGHT - 10) == GL_GREEN);

    // Blue rectangle in center of screen
    gl_draw_rect(WIDTH / 2 - 100, HEIGHT / 2 - 50, 200, 100, GL_BLUE);
    gl_draw_rect(WIDTH / 2 - 150, HEIGHT / 2 - 100, 100, 200, GL_YELLOW);

    // Single amber character
    gl_draw_char(60, 10, 'A', GL_AMBER);

    // Show buffer with drawn contents
    gl_swap_buffer();
    pause("Now displaying 1280 x 720, purple bg, single green pixel, blue center rect, amber letter A");

    gl_clear(GL_INDIGO);
    gl_draw_string(100, 100, "Hey everyone!", GL_CAYENNE);
    gl_draw_string(100, 120, "Let's test some weird drawing cases", GL_AMBER);

    gl_draw_string(300, 100, "This text is below", GL_GREEN);
    gl_draw_string(300, 120, "And this is on top!", GL_RED);

    gl_draw_string(470, 100, "Clipped? This text should be", GL_RED);

    gl_draw_string(100, 200, "by Asanshay", GL_WHITE);

    gl_swap_buffer();
    pause("Now displaying 1280 x 720, testing edge cases");
*/}

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

    gl_draw_triangle(300, 200, 300, 500, 450, 130, GL_YELLOW);
    gl_draw_triangle(400, 550, 600, 550, 500, 300, GL_BLUE);

    gl_swap_buffer();
    pause("Now displaying WIDTH x HEIGHT, testing triangles");
}

static void test_console(void)
{
    console_init(25, 50, GL_CYAN, GL_INDIGO);
    pause("Now displaying console: 25 rows x 50 columns, bg indigo, fg cyan");

    // Line 1: Hello, world!
    console_printf("Hello, world!\n");

    // Add line 2: Happiness == CODING
    console_printf("Happiness");
    console_printf(" == ");
    console_printf("CODING\n");

    // Add 2 blank lines and line 5: I am Pi, hear me roar!
    console_printf("\n\nI am Pi, hear me v\b \broar!\n"); // typo, backspace, correction

    console_printf("\n\nThis following line uses \\r: \nNot this :(\rOnly this!\n");

    console_printf("\n\nTesting wrapping: 1234567890123456789012345678901234567890123456789012345678901234567890\n");
    pause("Console printfs");

    // Test Scrolling
    console_printf("\n\nTesting scrolling: \nI'd say this is my favorite assigmment so far\n");
    console_printf("\nIt was really fun to learn about graphics \nI might do something similar for my final project\n");
    pause("Scroll to next line 1");
    console_printf("\n\nHow many lines do I have left? \nKeep typing text!!\n");
    pause("Scroll to next line 2");
    console_printf("10\n9\n8\n7\n6\n5\n4\n3\n2\n1\nHopefully we scrolled by now!\nWe did? Awesome!");
    pause("Scroll to next line 3");

    // Clear all lines
    console_printf("\f");

    // Line 1: "Goodbye"
    console_printf("Goodbye!\n");
    pause("Console clear");
}

/* TODO: Add tests to test your graphics library and console.
   For the graphics library, test both single & double
   buffering and confirm all drawing is clipped to bounds
   of framebuffer
   For the console, make sure to test wrap of long lines and scrolling.
   Be sure to test each module separately as well as in combination
   with others.
*/

void main(void)
{
    timer_init();
    uart_init();
    uart_putstring("OEHOEH#\n\n");
    printf("Executing main() in test_gl_console.c\n");

//    test_fb();
    test_gl();
//    test_gl_lines_triangles();
//    test_console();

    printf("Completed main() in test_gl_console.c\n");
}
