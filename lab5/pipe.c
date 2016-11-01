OFT *getFD();
PIPE *getPipe();
char *MODE[ ]={"READ_PIPE ","WRITE_PIPE"};

int show_pipe(PIPE *p)
{
   int i, j = p->tail;
   printf("------------ PIPE CONTENETS ------------\n");  
   
    for (i=p->data; i>0; i--)
    {
        if (i > PSIZE)
        {
            printf("Invalid pipe address!\n");
            break;
        }
        
        if (j == PSIZE) {j = 0;}
        printf("%c", p->buf[j++]);
    }
      
   printf("\n----------------------------------------\n");
   
   return 0;
}

int pfd()
{
	int i = 0;
	int j;
	OFT *p;
	
	printf("Proc %d file descriptors:\n", running->pid);
	printf("Index   Mode     Pipe         Ref   Contents\n");
	for(;i<NFD;i++)
	{		
		//if we have an opened proc
		if(running->fd[i] != 0)
		{
			printf(" %u     ", i);
			p = running->fd[i];
			switch(p->mode)
			{
				case READ_PIPE: printf("R        "); break;
				case WRITE_PIPE: printf("W        "); break;
				default: printf("Unknown:%d ", p->mode);
			}
			printf("%x      %d    ", p->pipe_ptr, p->refCount);
			
			
			j = 0;
			while(j < PSIZE)
			{
				printf("%c",p->pipe_ptr->buf[j]);
				j++;
			}
			
			printf("\n");
		}
	}
	
	printf("---------------------------------------------\n");
}

int read_pipe(int fd, char *buf, int n)
{
	//get the pipe from the running File Descriptor
	PIPE *p = running->fd[fd]->pipe_ptr;
	int r = 0;
	
	//if the file descriptor is not active
	if(running->fd[fd] == 0)
	{
		printf("Bad index:%d\n", fd);
		//return -1 on bad
		return 0;
	}
	
	//if our pipe is a write pipe
	if(running->fd[fd]->mode != READ_PIPE)
	{
		printf("Invalid File Descriptor. Must be a READ mode\n");
		return 0;
	}
	
	if(n <= 0) { return 0; }
	
	//while we have bytes requested to be read
	while(n)
	{
		printf("P:%d: READ: ", running->pid);
		//if there is data to be read
		while(p->data)
		{
			//put the byte from the buffer into
			//the buffer passed in
			put_byte(p->buf[p->tail], running->uss, buf);
			printf("%c", p->buf[p->tail]);
			p->tail++;
			buf++;
									
			//circular buffer, if we hit the end, we write
			//to the beginning
			if(p->tail == PSIZE) { p->tail = 0; }

			
			n--; r++;
			p->data--; p->room++;
			
			//if we still have data to be read
			//but the requested limit is satisfied
			if(n==0) {break;}
		}
		
		printf("\n");
		
		//wake up writers
		kwakeup(&(p->room));

		//if all data has been read
		if(n == 0 )
		{
			return r;
		}
		
		//if we have writers that are subscribing to this
		//we wake them up
		if(p->nwriter)
		{			
			//sleep on data
			printf("sleeping til more data...\n");
			ksleep(&(p->data));
			continue;
		}
		
		tswitch();	
	}
}

int write_pipe(int fd, char *buf, int n)
{
	PIPE *p = running->fd[fd]->pipe_ptr;
	char str[1024];
	int r = 0; int size = 0;
	
	//if the file descriptor is not active
	if(running->fd[fd] == 0)
	{
		printf("Bad index:%d\n", fd);
		//return -1 on bad
		return 0;
	}
	
	//if our pipe is a read pipe
	if(running->fd[fd]->mode != WRITE_PIPE)
	{
		printf("Invalid File Descriptor. Must be a WRITE mode\n");
		return 0;
	}
	
	if(n<=0) { return 0; }
	
	//while we have not satisfied our limit
	while(n)
	{	
		printf("P:%d: WROTE: ", running->pid);
		if(!p->nreader)
		{
			kexit(30);	
		}
		
		while(p->room)
		{
			//write from buf to pipe
			p->buf[p->head] = get_byte(running->uss, buf);
			printf("%c", p->buf[p->head]);
			buf++;
			p->head++;
			
			//circular buffer, if we hit the end, we write
			//to the beginning
			if(p->head == PSIZE) { p->head = 0; }
						
			r++; p->data++; p->room--; n--;
			if(n==0) {break;}
		}	
		
		printf("\n");
		
		printf("wakeup all other procs\n");
		kwakeup(&(p->data));
		
		if(n==0)
		{
			return r;
		}
		
		if(p->nreader)
		{
			printf("sleeping til more room...\n");
			ksleep(&(p->room));	
		}
		
		tswitch();
	}
}

int kpipe(int pd[2])
{
	PIPE *p;
	OFT *r = 0; OFT *w = 0;
	int i = 0; int j = 0;
		
	r = getFD();
	r->refCount = 1;
	w = getFD();
	w->refCount = 1;
	p = getPipe();
	
	//find a position for the fd in p to store the
	//read and write oft
	while(i < NFD && (running->fd[i] != 0)) { i++; }
	j = i+1;
	while(j < NFD && (running->fd[j] != 0)) { j++; }
	
	//basically, if we do not receieve two OFTs and a PIPE
	//and our index for inserting the OFTs is not valid,
	//we have to quit the pipe operation
	if(r == 0 || w == 0 || p == 0 || i == NFD || j == NFD)
	{
		r->refCount = 0;
		w->refCount = 0;
		
		//returns -1 on failure
		return -1;
	}
	
	//insert the Read and Write OFT
	running->fd[i] = r;
	running->fd[j] = w;
		
	//write the index to the PD
	put_word(i, running->uss, pd++);
	put_word(j, running->uss, pd);
	
	//set up the Pipe
	p->head = 0;
	p->tail = 0;
	p->data = 0;
	p->room = PSIZE;
	p->nwriter = 1;
	p->nreader = 1;
	p->busy = 1;
	
	//set up the OFTs
	r->mode = READ_PIPE;
	w->mode = WRITE_PIPE;
	r->pipe_ptr = p;
	w->pipe_ptr = p;
	
	//return 0 on success
	return 0;
}

int close_pipe(int fd)
{
  OFT *op; PIPE *pp;

  printf("proc %d close_pipe: fd=%d\n", running->pid, fd);

  op = running->fd[fd];
  
  if (op->mode == READ_PIPE)
  {
      pp = op->pipe_ptr;
      pp->nreader--;                       // dec n reader by 1
	  op->refCount--;

      if (pp->nreader <= 0)		       //if we have removed the last reader from the pipe
      {        
			if (pp->nwriter <= 0)	  	   //if there are no more writers
			{        
				pp->busy = 0;              // free the pipe   
			}
      }
      
      kwakeup(&(pp->room));                  // wakeup any WRITER on pipe 
  }
  
  // YOUR CODE for the WRITE_PIPE case:
  else if (op->mode == WRITE_PIPE)
  {
      // decrement the writer by 1
      pp = op->pipe_ptr;
      pp->nwriter--;
      op->refCount--;
        
      // if this is the last writer and we have no more readers free the pipe
      if (pp->nwriter <= 0)
      {
		  if(pp->nreader <= 0) 	//if there are also no more readers
		  {
			pp->busy = 0;  		//the pipe is not busy and should be freed
		  }
		  
		  // wakeup procs
		  kwakeup(&(pp->data)); 
      }
  }
        
  running->fd[fd] = 0;
  return;
}

// gets a free file descriptor
OFT *getFD()
{
    OFT *f = 0;
    int i;
    
    for (i= 0; i < NFD; i++)
    {
        f = &oft[i++];
        if (f->refCount <= 0)
        {
			f->refCount = 0;
            return f;
		}
    }
    
    printf("Could not receieve an open FD\n");
    return 0;
}

// gets a free pipe
PIPE *getPipe()
{
    PIPE *p;
    int i = 0;
    
    while (i < NPIPE)
    {
        p = &pipe[i++];
        if (p->busy == 0)
        {
            return p;
		}
    }
    
    printf("Could not get a free Pipe\n");
    return 0;
}
