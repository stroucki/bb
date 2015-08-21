
#include "asmdefs.h"

.text



.globl __irq_wrapper                                                     
   .align 4                                                             
__irq_wrapper:                                                         
/*  cli*/
   pushw %ds
   pushl %eax
   movl $0x20,%eax
   pushl %ebx
									  
   outb %al,$0x20
   .byte 0x2e                             /* cs: override */             
   movw ___djgpp_ds_alias, %ds
									
   movl ___pcspe,%eax
   outb %al,$97
   movl __ptr,%ebx
   xorl $1,%eax
   outb %al,$97
   movl (%ebx),%eax
   incl %ebx
   outb %al,$66

   cmpl %ebx,__ptr_end
   ja noend
   movl __ptr_start,%ebx
   incl __counter
   .align 4
noend:
   movl %ebx,__ptr
/*
   movl __delay,%ebx
   addl %ebx,__bios
   js bios
*/
   popl %ebx
   popl %eax
   popw %ds
/*  sti*/
   iret 
/*
   .align 4
bios:
   addl $0x10000,__bios
   popl %ebx
   popl %eax
   popw %ds
   ljmp %cs:__oldirq_handler                        
*/
   .align 4
.globl __irq_wrapper_end
__irq_wrapper_end:
   ret
