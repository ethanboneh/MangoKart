#include "accel_driver.h"
#include "assert.h"
#include "i2c.h"
#include "printf.h"
#include "strings.h"
#include "uart.h"

enum reg_address {
    WHO_AM_I  = 0x75,
    CONFIG = 0x1A,
    ACCEL_CONFIG = 0x1C,
    USER_CTRL = 0x6A,
    SLV0_ADDR = 0x25,
    SLV0_REG = 0x26,
    SLV0_CTRL = 0x27,
    SLV0_DO = 0x63,
    // CTRL1_XL  = 0x10,
    // CTRL8_XL  = 0x17,
    // CTRL9_XL  = 0x18,
    A_OUTX_L_XL = 0x3C,
    A_OUTX_H_XL = 0x3B,
    A_OUTY_L_XL = 0x3E,
    A_OUTY_H_XL = 0x3D,
    A_OUTZ_L_XL = 0x40,
    A_OUTZ_H_XL = 0x3F,
    G_OUTX_L_XL = 0x44,
    G_OUTX_H_XL = 0x43,
    G_OUTY_L_XL = 0x46,
    G_OUTY_H_XL = 0x45,
    G_OUTZ_L_XL = 0x48,
    G_OUTZ_H_XL = 0x47,
    PWR_MGMT_1 = 0x6B,
    PWR_MGMT_2 = 0x6C
};

static const unsigned MY_I2C_ADDR = 0x68; // confirm device id, components can differ!

static void write_reg(unsigned char reg, unsigned char val) {
	unsigned char data[2] = {reg, val};
	i2c_write(MY_I2C_ADDR, data, 2);
}

static unsigned read_reg(unsigned char reg) {
	i2c_write(MY_I2C_ADDR, &reg, 1); // how does this code read?
	unsigned char val = 0;
	i2c_read(MY_I2C_ADDR, &val, 1);
	return val;
}

void mpu_init(void) {
    unsigned id = read_reg(WHO_AM_I);  // confirm id, expect 0x69
    assert(id == 0x68);

    write_reg(SLV0_CTRL, 0b10001100); // configure i2c
    unsigned char pwr = read_reg(PWR_MGMT_1);
    write_reg(PWR_MGMT_1, 0);
    // printf("before: %x \n", pwr);
    // pwr = 0b00100000|(pwr & 0b1001111); 
    // printf("%x \n", pwr);
    // write_reg(PWR_MGMT_1, pwr); // get out of sleep mode
    unsigned char new = read_reg(PWR_MGMT_1);
    printf("write: %x \n", new);
	// write_reg(CTRL1_XL, 0x80);  // 1600Hz (high perf mode)
    // accelerator _XL registers
    // write_reg(CTRL9_XL, 0x38);  // ACCEL: x,y,z enabled (bits 4-6)
    unsigned char pwr2 = read_reg(PWR_MGMT_2);
    printf("pwr2: %x \n", pwr2);
    write_reg(PWR_MGMT_2, 0xC0);
    pwr2 = read_reg(PWR_MGMT_2);
    printf("pwr2 after: %x \n", pwr2);
    unsigned char x =  read_reg(A_OUTX_L_XL);
    printf("x accel: %x \n", x);
    printf("zero: %x \n", 0);
}

void mpu_read_accelerometer(short *x_a, short *y_a, short *z_a, short *x_g, short *y_g, short *z_g) {
    *x_a =  read_reg(A_OUTX_L_XL);
    *x_a |= read_reg(A_OUTX_H_XL) << 8;
    *y_a =  read_reg(A_OUTY_L_XL);
    *y_a |= read_reg(A_OUTY_H_XL) << 8;
    *z_a =  read_reg(A_OUTZ_L_XL);
    *z_a |= read_reg(A_OUTZ_H_XL) << 8;
    
    *x_g =  read_reg(G_OUTX_L_XL);
    *x_g |= read_reg(G_OUTX_H_XL) << 8;
    *y_g =  read_reg(G_OUTY_L_XL);
    *y_g |= read_reg(G_OUTY_H_XL) << 8;
    *z_g =  read_reg(G_OUTZ_L_XL);
    *z_g |= read_reg(G_OUTZ_H_XL) << 8;
}
