/* Linker script to configure memory regions. */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x00038000, LENGTH = 0x80000   /* 512k */
}
/*
SECTIONS
{
	.rodata : ALIGN(4)
	{
		FwInfo.o(.rodata .rodata.*)
	} > FLASH

}
*/
SECTIONS
{
  . = 0x00038000;
  .text : { *(.text) }
  . = 0x2000000;
  .data : { *(.data) }
  .bss : { *(.bss) }
}
