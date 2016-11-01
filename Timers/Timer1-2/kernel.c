int body();
int goUmode();
int kmode();

//delimits string based on the delimiter and returns an array of strings
char **myStringTok(char *s, char *del)
{
	int i = 0;
	char *tokens[64];
	char *token;
	
	//get the tokens out of the string based on the delim
	token = strtok(s, del);

	i = 0;
	while(token && i < 64)
	{
		//set each pointer to the array of pointers
		tokens[i] = token;
		//get new pointer and increment
		token = strtok(0, del);
		i++;
	}
	
	//set end to the tokens
	tokens[i] = 0;
	
	//return a pointer to a pointer of chars
	return tokens;	
}

//used for using a whole new image to a new process
PROC *kfork(char *filename) // create a child that begins from body
{
	//REGULAR KFORK STUF
	int i;
	PROC *p = get_proc(&freeList);
	u16 segment;

	if (!p) {
		printf("no more PROC, kfork() failed\n");
		return 0;
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

	//point kstack at umode
	p->kstack[SSIZE - 1] = (int)goUmode;
	
	//make sp point to top of stack
	p->ksp = &p->kstack[SSIZE - 9];
	
	enqueue(&readyQueue, p);
	
	//copy file descriptors from running to the child
	for(i = 0; i < NFD; i++)
	{
		if(running->fd[i])
		{
			p->fd[i] = running->fd[i];
			p->fd[i]->refCount++;
			  
			if(p->fd[i]->mode == READ_PIPE)
			{
				p->fd[i]->pipe_ptr->nreader++;
			}
			  
			if(p->fd[i]->mode == WRITE_PIPE)
			{
				p->fd[i]->pipe_ptr->nwriter++;
			}
		}
	}
	
	//NEW KFORK STUFF
    //if a filename for uImage is passed in
    if (filename)
    {
	  segment = (p->pid + 1) * 0x1000; // Create the user mode for this process!
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

	return p;
}

int kgetpid()
{
	if(running->pid)
	{
		return running->pid;
	}
	
	else {return -1;}
}

int kps()
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

int chname(char *name)
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

int copyImage(u16 pseg, u16 cseg, u16 size)
{
	u16 i;
	for(i = 0; i < size; i++)
	{ 
		put_word(get_word(pseg, 2*i), cseg, 2*i); 
	}
}

//returns kfork
//used when keeping image the same
int fork()
{
  int pid; u16 segment;
  PROC *p = 0;
  
  //get the proc that was forked from the original image
  //we pass in  a null value because we do not need a new image
  //instead, we use the current image
  p = kfork(0);
    
  //if we have a null pointer, we return 1 to indicate failure
  if(p == 0) { return -1; }
  
  //else if we have a number, that number is our child proc
  //get the segment that our proc's umode is located at.
  segment = (p->pid+1) * 0x1000;
  
  //copies 32k words form the running (or parent's) umode into
  //the child's umode
  copyImage(running->uss, segment, 32*1024);
  
  //set up the childs uss and usp
  p->uss = segment;
  p->usp = running->usp; //since the images are the same, so is the offset (sp)
  
  //change the uDS, uES, uCS, and the return (ax) in the child stack
  put_word(segment, segment, p->usp);   	//uDS = segment
  put_word(segment, segment, p->usp+2); 	//uES = segment
  put_word(0,		segment, p->usp+2*8); 	//uax = 0
  put_word(segment, segment, p->usp+2*10);  //uCS = segment

  return p->pid; // return child PROC pointer
}

int kmode()
{
	//running->inkmode = 1;
	printf("P[%d] going to kernel\n", running->pid);
	body();
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

//this one has a stack change function
int kexec(char *y)
{
	int i, length = 0; int offsetLength;
	char filename[64], *cp = filename;
	char tokens[64], token[64];
	char *yP = *y;
	u16 segment = running->uss;
		
	//get the filename from the umode
	//what is happening is...
	//we get the pointer byte by byte from the umode and add it to
	//the filename buffer pointed to by the cp pointer.
	//our limit is 64 bytes btw.
	while((*cp++ = get_byte(running->uss, y++)) && (length++ < 64));
	
	//terminate the string
	//reset to the start of filename
	*cp = '\0';
	cp = &filename[0];
		
	//copy the two arrays together
	strcpy(tokens, filename);
	
	//append /bin/ to the image name
	token[0] = '/';
	token[1] = 'b';
	token[2] = 'i';
	token[3] = 'n';
	token[4] = '/';

	for(i = 0; i < length && filename[i] != ' '; i++) { token[i + 5] = filename[i]; }
	token[i + 5] = '\0';
	
	
	//if we cannot load the filename into the segment
	if(!load(token, segment)) { return -1; }	
	
	
	//if our length is odd, then we increment it to add a buffer
	//INCREMENT TO REMEMBER THE NULL CHARACTER!!!
	length++;
	if(length % 2 != 0)
	{		
		//pad string
		filename[length] = ' ';
		filename[length+1] = '\0';
		length++;
	}
	
	//now we have to reiniatlize the process' ustack for it
	//to return to VA=0
	for(i=1; i<=(12 + length); i++)
	{
		//clear the stack at the segment
		put_word(0, segment, -2*i);
	}
	
	//SET UP STACK TO INCLUDE WORD
	//set the usp to the top end of the uss stack
	//keep everything 0 besides flag. uDS and eDS and uSS all still seg!
	running->usp = 	-24  - (2+length);
	put_word(segment, segment, -2*12 - (2+length)); //saved uds
	put_word(segment, segment, -2*11 - (2+length)); //saved eds
	put_word(segment, segment, -2*2 - (2+length));  //uCS = segment; uPC = 0;
	put_word(0x200, segment, -2*1 - (2+length));    //Flag = 00x200. sets the Tracer and Halt bit
	
	//set up the string and the s
	put_word(-(length), segment, -(2+length));
	
	//push the string, push on the null character
	for(i = 0; i < length-1; i++)
	{
		put_byte(filename[i],segment, -(length)+i);  
	}
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
