OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00002000 /* Flash, 8K */
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00002000 /* Memory, 8K */
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        _data = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > SRAM

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM

    . = ALIGN(4);
    _end = . ;

    /* Reserve remaining memory for JITed code buffer */
    . = ALIGN(32);
    __CodeBuffer = .;

    __StackTop = ORIGIN(SRAM) + LENGTH(SRAM);
    __StackLimit = __StackTop - 0x400;
    PROVIDE(_stack = __StackLimit);
    PROVIDE(_estack = __StackTop);
    PROVIDE(_scode_buffer = __CodeBuffer);
    PROVIDE(_ecode_buffer = __StackLimit);
}
