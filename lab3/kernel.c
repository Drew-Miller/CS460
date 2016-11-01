int body();

int kfork(char *filename) // create a child that begins from body
{
	//REGULAR KFORK STUF
	int i;
	PROC *p = get_proc(&freeList);
	u16 segment;

	if (!p) {
		printf("no more PROC, kfork() failed\n");
		return -1;
	}
	
	//if we got a new proc, we first have to set it up
	p->status = READY;
	p->priority = 1;
	p->ppid = running->pid;
	p->parent = running;

	//SET ALL KSTACK REGISTERS TO NOTHING
	for (i = 1; i < 10; i++)
	{  
		p->kstack[SSIZE - i] = 0;
	}

	//point kstack at body
	p->kstack[SSIZE - 1] = (int)body;
	
	//mae sp point to top of stack
	p->ksp = &p->kstack[SSIZE - 9];
	
	enqueue(&readyQueue, p);
	
	//NEW KFORK STUFF
	segment = (p->pid + 1) * 0x1000; // Create the user mode for this process!

	//if a filename for uImage is passed in
	if (filename)
	{
		//load the file
		load(filename, segment);
		
		// set flag
		put_word(0x0200, segment, -2);
		put_word(segment, segment, -4);
		
		//set the registers to null
		put_word(0, segment, -6);
		put_word(0, segment, -8);
		put_word(0, segment, -10);
		put_word(0, segment, -12);
		put_word(0, segment, -14);
		put_word(0, segment, -16);
		put_word(0, segment, -18);
		put_word(0, segment, -20);
		
		// uCS = segment
		put_word(segment, segment, -22);
		// uDS = segment 
		put_word(segment, segment, -24);
	
		p->uss = segment;
		p->usp = -24;
	}

	return p->pid; // return child PROC pointer
}
