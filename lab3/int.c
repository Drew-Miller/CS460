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
	
   printf("In Kernel for Syscall\n");
   
   a = get_word(running->uss, running->usp + 2 * PA);
   b = get_word(running->uss, running->usp + 2 * (PA + 1));
   c = get_word(running->uss, running->usp + 2 * (PA + 2));
   d = get_word(running->uss, running->usp + 2 * (PA + 3));

   switch(a){
       case 0 : r = kgetpid();        break;
       case 1 : r = kpd();            break;
       case 2 : r = kchname(b);       break;
       case 3 : r = kkfork();         break;
       case 4 : r = ktswitch();       break;
       case 5 : r = kkwait(b);        break;
       case 6 : r = kkexit(b);        break;
       case 7 : r = kgetc();		  break;
       case 8 : r = kputc(b);         break;
       case 99: kkexit(b);            break;
       default: printf("invalid syscall # : %d\n", a); 
   }

	put_word(r, running->uss, running->usp + 16);
	return r;
}

//============= WRITE C CODE FOR syscall functions ======================

int kgetpid()
{
	if(running->pid)
	{
		return running->pid;
	}
	
	else {return -1;}
}

int kpd()
{
	PROC *p = 0;
	int i;
	
	for(i = 0; i < NPROC; i++)
	{
		
		p = &proc[i];
		
		printf("PROC: %s : pid-> %d  ppid-> %d ", p->name, p->pid, p->ppid);
		
		switch(p->status)
		{
			case FREE: printf("FREE "); break;
			
			case READY: printf("READY "); break;
			
			case RUNNING: printf("RUNNING "); break;
			
			case STOPPED: printf("STOPPED "); break;
			
			case SLEEP: printf("SLEEP "); break;
			
			case ZOMBIE: printf("ZOMBIE "); break;		
		}
		
		printf("\n");
	}
	
	return 0;
}

int kchname(char *name)
{
	char c;
	int i = 0;
	while (i < 32)
	{
		c = get_byte(running->uss, name + i);
		running->name[i] = c;
		i++;
	}
	
	return 1;
}

//returns kfork
int kkfork()
{
	//"/bin/u1"
  int pid = kfork("/bin/u1");
  
  return pid;
}

int ktswitch()
{
    return tswitch();
}

int kkwait(int *status)
{
	int c;
	int pid = kwait(&c);
	
	put_word(c, running->uss, status);
	
	return pid;
}

int kkexit(int value)
{
    return kexit(value);
}

//returns a char from kernel
int kgetc()
{
	char c;
	c = getc();
	return (int)c;
}

int kputc(char c)
{
	putc(c);
}
