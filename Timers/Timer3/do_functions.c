void do_kfork()
{
	printf("doing kfork\n");
	fork();
}

void do_tswitch()
{
	printf("doing switch\n");
	tswitch();
}

void do_wait()
{
	int pid, status;
	pid = kwait(&status);
	printf("pid = %d with exit %d\n", pid, status);
}

void do_exit()
{
	int e;
	char buf[32];
	if(running->pid == 1)
	{
		printf("Can't exit P1\n");
		return;
	}
	
	printf("Exit Value?\n");
	gets(buf);
	e = myAtoi(buf);
	kexit(e);
}

void do_pid()
{
	printf("PID of RUNNING: %d\n", running->pid);
}

void do_ps()
{
	kps();
}

void do_umode()
{
	//running->inkmode = 0;
	goUmode();
}

//turns string to int. returns 0 if not a full int
int myAtoi(char *s)
{
	int result;
	int i;
	result = 0;
	i = 0;
			
	while(*s)
	{
		//if we have an integer number
		if(*s >= 8 && *s <= 57)
		{
			//we multiply the result by a factor of ten and add the
			//new value from the string to it
			result = result * 10 + (int)(s[i] - '0');
			s++;
		}
		
		//returns 0 if s is not all ascii integers
		else {return 0;}
	}
	
	return result;
}

int kexit(int exitValue)
{
	PROC *p = 0;
	int i, wakeupP1 = 0;
	
	//if p1 has child procs, we cannot exit right now
	if(running->pid == 1)
	{
		printf("CANNOT QUIT P1\n");
		
		return -1;
	}
	
	for(i = 1; i < NPROC; i++)
	{
		p = &proc[i];
		
		//if we have a p that if not free and its parent
		//is the running proc, we set the procs parent to 1
		if((p->status != FREE) && (running->pid == p->ppid))
		{
			p->ppid = 1;
			p->parent = &proc[1];
			wakeupP1++;
		}	
	}
	
	//record exitValue and become a ZOMBIE
	running->exitCode = exitValue;
	running->status = ZOMBIE;
	
	//wakeup parent and also P1 if necessary
	kwakeup(running->parent);
	printf("RETURN to Parent->id: %d\n", running->parent->pid);
	if(wakeupP1)
	{
		kwakeup(&proc[1]);
	}
	
	//give up cpu
	tswitch();
	return 0;
}

