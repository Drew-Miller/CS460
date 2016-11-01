/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/
int kcinth()
{
   int a,b,c,d, r;
   int PA = 13;
      
   a = get_word(running->uss, running->usp + 2 * PA);
   b = get_word(running->uss, running->usp + 2 * (PA + 1));
   c = get_word(running->uss, running->usp + 2 * (PA + 2));
   d = get_word(running->uss, running->usp + 2 * (PA + 3));
   
   switch(a){
       case 0 : r = running->pid;   	break;
       case 1 : r = kps();          	break;
       case 2 : r = chname(b);      	break;
       case 3 : r = kmode();        	break;
       case 4 : r = tswitch();      	break;
       case 5 : r = kkwait(b);      	break;
       case 6 : r = kkexit(b);      	break;
       case 7 : r = fork();         	break;
       case 8 : r = kexec(b);       	break; 
        
       case 30: r = kpipe(b);			break;
       case 31: r = read_pipe(b,c,d);	break;
       case 32: r = write_pipe(b,c,d);	break;
	   case 33: r = close_pipe(b);		break;
	   case 34: r = pfd();				break;       
       
       case 90: r = kgetc();         	break;
       case 91: r = kputc(b);        	break; 
             
       case 99: kexit(b);           	break;
       default: printf("invalid syscall # : %d\n", a); 
   }

	put_word(r, running->uss, running->usp + 16);
	return r;
}
