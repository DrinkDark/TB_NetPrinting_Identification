MEMORY
{
	FLASH (rx)  : ORIGIN = 0x08010000, LENGTH = 64K
	RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
	.text :
	{
		_stext = .;          /* Provide the name for the start of this section */
		
		CREATE_OBJECT_SYMBOLS
		KEEP(*(.header))
		
		*(.text)
		*(.text.*)
				
		. = ALIGN(4);        /* Align the start of the rodata part */
		*(.rodata)
		*(.rodata.*)
		
		. = ALIGN(4);        /* Align the end of the section */
	} > FLASH = 0
	_etext = .;             /* Provide the name for the end of this section */
	
	
	.data : AT (_etext)
	{
		. = ALIGN(4);        /* Align the start of the section */
		_sdata = .;          /* Provide the name for the start of this section */
		
		*(.data)
		*(.data.*)
		
		. = ALIGN(4);        /* Align the start of the fastrun part */
		*(.fastrun)
		*(.fastrun.*)
		
		. = ALIGN(4);        /* Align the end of the section */
	} > RAM
	_edata = .;             /* Provide the name for the end of this section */
	
	.bss :
	{
		. = ALIGN(4);        /* Align the start of the section */
		_sbss = .;           /* Provide the name for the start of this section */
		
		*(.bss)
		*(.bss.*)
		
		. = ALIGN(4);        /* Align the end of the section */
	} > RAM
	_ebss = .;              /* Provide the name for the end of this section */
	end = .;		/* heap starts here */
	
	_estack = ORIGIN(RAM) + LENGTH(RAM);
}
