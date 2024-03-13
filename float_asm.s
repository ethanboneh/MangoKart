.globl config_float
config_float:
    li a0,1<<13
    csrs mstatus,a0
