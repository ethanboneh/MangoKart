# Sample makefile for project
# Builds "3dgl.bin" from 3dgl.c (edit PROGRAM to change)
# Additional source file(s) 3dgl.c (edit SOURCES to change)
# Link against your libmango + reference libmango (edit LDLIBS, LDFLAGS to change)

RUN_PROGRAM = play_game.bin
TEST_PROGRAM  = test_graphics.bin

MY_MODULE_SOURCES = 3dgl.c # $(PROGRAM:.bin=.c) $(TEST_PROGRAM:.bin=.c) 


PROGRAMS      = $(RUN_PROGRAM) $(TEST_PROGRAM)

all: $(PROGRAMS)

# Flags for compile and link
ARCH    = -march=rv64im -mabi=lp64 # lp64d
ASFLAGS = $(ARCH)
CFLAGS  = $(ARCH) -g -Og -I$$CS107E/include $$warn $$freestanding -fno-omit-frame-pointer
LDFLAGS = -nostdlib -L$$CS107E/lib -T memmap.ld
LDLIBS  = -lmango -lmango_gcc

# Common objects for the programs built by this makefile
SOURCES = $(MY_MODULE_SOURCES)
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

# Rules and recipes for all build steps

# Extract raw binary from elf executable
%.bin: %.elf
	riscv64-unknown-elf-objcopy $< -O binary $@

# Link program executable from program.o and all common objects
%.elf: $(OBJECTS) %.o
	riscv64-unknown-elf-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile C source to object file
%.o: %.c
	riscv64-unknown-elf-gcc $(CFLAGS) -c $< -o $@

# Assemble asm source to object file
%.o: %.s
	riscv64-unknown-elf-as $(ASFLAGS) $< -o $@

# Disassemble object file to asm listing
%.list: %.o
	riscv64-unknown-elf-objdump $(OBJDUMP_FLAGS) $<

# Build and run the application binary
run: $(RUN_PROGRAM)
	mango-run $<

# Build and run the test binary
test: $(TEST_PROGRAM)
	mango-run $<

# Remove all build products
clean:
	rm -f *.o *.bin *.elf *.list

# this rule will provide better error message when
# a source file cannot be found (missing, misnamed)
$(SOURCES):
	$(error cannot find source file `$@` needed for build)

libmymango.a:
	$(error cannot find libmymango.a Change to mylib directory to build, then copy here)

.PHONY: all clean run
.PRECIOUS: %.elf %.o

# disable built-in rules (they are not used)
.SUFFIXES:

export warn = -Wall -Wpointer-arith -Wwrite-strings -Werror \
              -Wno-error=unused-function -Wno-error=unused-variable \
              -fno-diagnostics-show-option
export freestanding = -ffreestanding -nostdinc \
                      -isystem $(shell riscv64-unknown-elf-gcc -print-file-name=include)
