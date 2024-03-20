#pragma once
#include <stdbool.h>

void mpu_init(void);
void mpu_read_accelerometer(short *x_a, short *y_a, short *z_a, short *x_g, short *y_g, short *z_g);
