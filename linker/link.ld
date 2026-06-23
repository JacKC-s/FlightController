ENTRY(Reset_Handler);
MEMORY{
    flash(rx) : ORIGIN = 0x08000000, LENGTH = 512k
    sram(rwx) : ORIGIN = 0x20000000, LENGTH = 128k
}
_estack = ORIGIN(sram) + LENGTH(sram);

/*Didnt link the C libraries*/
SECTIONS {
    .vectors : {KEEP(*(.isr_vector))} > flash
    .text : {
        *(.text*)
        KEEP(*(.init)) // C libraries linked
        KEEP(*(.fini))
    } > flash 

    /* C library init/fini — needed by __libc_init_array in startup - AI Found this bug, dont know if it works */
    .preinit_array : {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > flash
    .init_array : {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > flash
    .fini_array : {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > flash
    
    .rodata : {*(.rodata*)} > flash

    .data : {
        _sdata = .;
        *(.first_data)
        *(.data SORT(.data.*))
        _edata = .;
    } > sram AT > flash
    _sidata = LOADADDR(.data);

    .bss : {
        _sbss = .;
        *(.bss SORT(.bss.*) COMMON)
        _ebss = .;
    } > sram

    . = ALIGN(8);
    _end = .;
}