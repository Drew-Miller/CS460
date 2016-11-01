        .globl _main0,_syscall,_exit,_getcs
        .globl _getc, _putc, _color


export auto_start	
auto_start:		
         call _main0
		! if ever return, exit(0)
		push  #0
        call  _exit

_syscall:
        int    80
        ret

	
_getcs:
        mov   ax, cs
        ret
