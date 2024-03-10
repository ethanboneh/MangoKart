#include "uart.h"
#include "3dgl.h"

void main(void)
{
    uart_init();
    say_hello("CS107e");
}
