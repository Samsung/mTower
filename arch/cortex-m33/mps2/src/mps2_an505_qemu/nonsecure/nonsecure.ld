__STACK_SIZE = 0x00004000;

MEMORY
{
   NS_FLASH     (rx)  : ORIGIN = 0x00200000, LENGTH = 224K
   NS_FLASH_END (rx)  : ORIGIN = 0x00260000, LENGTH = 512K
   NS_RAM       (rwx) : ORIGIN = 0x00238000, LENGTH = 16K
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Entry Point (reset) */
ENTRY(Reset_Handler)

SECTIONS
{
  .text : 
  {
    /* Make sure the vector is the first point of the project. */
    KEEP(*(.vectors))
    *(.text)
    *(.data)
  	*(.bss) 
  } > NS_FLASH

  .ARM.exidx :
  {
    __exidx_start = .;
     *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    __exidx_end = .;
  } > NS_FLASH
  /* define a global symbol _etext just after the last code byte */
  _etext = .;

  .heap (COPY):
	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		KEEP(*(.heap*))
		__HeapLimit = .;
	} > NS_FLASH
  /*
  
  /* Set __StackTop for the vector table first index. */
  __StackTop = ORIGIN(NS_FLASH_END) + LENGTH(NS_FLASH_END);
  /*
   * Stack is descending, meaning that putting values in the stack will decrease
   * so the limit of the stack is less than the base size address.
   */
  __StackLimit = __StackTop - __STACK_SIZE;
}
