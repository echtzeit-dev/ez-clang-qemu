.DEFAULT_GOAL := bin/ez-clang-qemu.bin

ifeq ($(DEBUG),1)
	opt_flags := -Og -g2 -ggdb2
else
	opt_flags := -Os -g0 -ffunction-sections -fdata-sections
endif

device_flags := -mcpu=cortex-m3 -mthumb -DPART_LM3S811 -Dgcc
freestanding_flags := -nostdlib -nostartfiles -ffreestanding
source_files := src/ez-clang-qemu.c src/$(TARGET_DEVICE).c
object_files := $(source_files:%=bin/%.o)
#$(info Source files: $(source_files))
#$(info Object files: $(object_files))

dir_guard=@mkdir -p $(@D)

bin/%.c.o: %.c $(dirs)
	$(dir_guard)
	$(CC) $(device_flags) $(opt_flags) $(freestanding_flags) -std=c99 -Wall -o $@ -c $<

bin/ez-clang-qemu.axf: $(object_files)
	$(LD) -T src/$(TARGET_DEVICE).ld --gc-sections --print-gc-sections -o $@ $(object_files)

bin/ez-clang-qemu.bin: bin/ez-clang-qemu.axf
	$(objcopy) $< $@ -O binary

.PHONY: clean
clean:
	rm -rf bin
