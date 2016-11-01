//ksleep
int ksleep(int event)
{	
	int i; PROC *p;
	running->event = event;
	running->status = SLEEP;
	tswitch();
}

//kwakeup
int kwakeup(int event)
{
	int i; PROC *p;
	for(i = 1; i<NPROC; i++)
	{
		p = &proc[i];
		if(p->status == SLEEP && p->event == event)
		{
			p->event = 0;
			p->status = READY;
			enqueue(&readyQueue, p);
			nproc--;
		}
	}
}

//writes exit code to status and returns pid of zombie
int kwait(int *status)
{
	PROC *p; int i, hasChild = 0;
	
	while(1)
	{
		//search PROCs for a child
		for(i = 1; i < NPROC; i++)
		{
			p = &proc[i];
			
			//if the proc is not free and its parent is running
			if(p->status != FREE && p->ppid == running->pid)
			{
				//we found a child of the proc and it is ready!
				if(p->status == READY)
				{
					hasChild = 1;
				}
				
				//if we found a zombie child, we can kill it
				if(p->status == ZOMBIE)
				{
					*status = p->exitCode;
					p->status = FREE;
					put_proc(&freeList, p);
					return (p->pid);
				}
			}
		}
		
		//if we have no suitable child to wait for
		if(!hasChild) { return -1; }
		ksleep(running);
	}
}
