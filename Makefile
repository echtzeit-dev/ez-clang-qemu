ifndef TARGET_DEVICE
$(error Missing device name. Please provide TARGET_DEVICE variable)
endif

ifeq ($(DEBUG),1)
	config_name := debug
	opt_flags := -Og -g2 -ggdb2
else
	config_name := release
	opt_flags := -Os -g0 -ffunction-sections -fdata-sections
	.DEFAULT_GOAL := $(TARGET_DEVICE)/release.bin
endif

.DEFAULT_GOAL := $(TARGET_DEVICE)/$(config_name).bin

device_flags := -mcpu=cortex-m3 -mthumb -DPART_LM3S811 -Dgcc
freestanding_flags := -nostdlib -nostartfiles -ffreestanding
source_files := src/ez-clang-qemu.c src/$(TARGET_DEVICE).c
object_files := $(source_files:%=$(TARGET_DEVICE)/%.o)
#$(info Source files: $(source_files))
#$(info Object files: $(object_files))

dir_guard=@mkdir -p $(@D)

$(TARGET_DEVICE)/%.c.o: %.c $(dirs)
	$(dir_guard)
	$(CC) $(device_flags) $(opt_flags) $(freestanding_flags) -std=c99 -Wall -o $@ -c $<

$(TARGET_DEVICE)/$(config_name).axf: $(object_files)
	$(LD) -T src/$(TARGET_DEVICE).ld --gc-sections --print-gc-sections -o $@ $(object_files)

$(TARGET_DEVICE)/$(config_name).bin: $(TARGET_DEVICE)/$(config_name).axf
	$(objcopy) $< $@ -O binary

clean:
	rm -rf $(TARGET_DEVICE)

# Default install location
firmware_dir := $(DESTDIR)/lib/ez-clang/$(VERSION)/firmware
install:
	find $(TARGET_DEVICE) -type f \( -iname \*.bin -o -iname \*.axf \) \
		-exec echo "$(firmware_dir)/{}"                 ';' \
		-exec install -Dm 755 "{}" "$(firmware_dir)/{}" ';'
