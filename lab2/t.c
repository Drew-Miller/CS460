typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define NPROC 9                // number of PROCs
#define SSIZE 1024             // per proc stack area 
#define NULL  0

#define FREE	 0
#define READY	 1
#define STOP	 2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5

typedef struct proc{
    struct proc *next;
    int    *ksp;               // saved ksp when not running
    int    pid;                // add pid for identify the proc
    int	   status;			   // status = FREE|READY|STOP|DEAD, etc
    int	   ppid;			   // parent pid
    struct proc *parent;       // pointer to parent proc
    int	   priority;		   // scheduling priority
    int	   event;			   // sleep event
    int    exitCode;           // exit code
    int    kstack[SSIZE];      // proc stack area
}PROC;


int procSize = sizeof(PROC);
int nproc; //keeps track of procs in the readyQueue
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;    // define NPROC procs
extern int color;

#include "bio.c"
#include "queue.c"

int do_tswitch();

int body()
{ 
   char c;
   printf("proc %d starts from body()\n", running->pid);
   while(1){
     color = running->pid + 7;
     
     printList("readyQueue", readyQueue);
     printList("freeList", freeList);
     
     printf("proc %d running : parent=%d\n", running->pid, running->priority,running->ppid);
     printf("enter a char [s|f|w|q]: ");
     c = getc(); printf("%c\n", c);
     switch(c)
     {
		case 'f': do_kfork(); break;
		case 's': do_tswitch(); break;
		case 'w': do_wait(); break;
		case 'q': do_exit(); break;
		default: printf("invalid command\n"); break;
	 }
   }
}

PROC *kfork()
{
	int i;
	PROC *p = get_proc(&freeList);
	if(!p)
	{
		printf("no more PROC, kfork() failed\n");
		return 0;
	}
	
	nproc++;
	p->status = READY;
	p->priority = 1;
	p->ppid = running->pid;
	p->parent = running;
	
	//initialize new proc's kstack
	for(i = 1; i < 10; i++)
	{
		p->kstack[SSIZE -i] = 0;
	}
	
	p->kstack[SSIZE - 1] = (int)body;
	
	//p->ksp = &p->kstack[SSIZE - 11];
	p->ksp = &p->kstack[SSIZE - 9];
	nproc++;
	enqueue(&readyQueue, p);
	return p;
}

int kexit(int exitValue)
{
	PROC *p = NULL;
	int i, wakeupP1 = 0;
	
	//if p1 has child procs, we cannot exit right now
	if(running->pid == 1 && nproc > 2)
	{
		printf("other procs still exist, P1 can't die yet\n");
		return -1;
	}
	
	for(i = 1; i < NPROC; i++)
	{
		p = &proc[i];
		
		//if we have a p that if not free and its parent
		//is the running proc, we set the procs parent to 1
		if((p->status != FREE) && (running->ppid == p->ppid))
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
	printf("running->parent->id: %d\n", running->parent->pid);
	if(wakeupP1)
	{
		kwakeup(&proc[1]);
	}
	
	//give up cpu
	tswitch();
}

//ksleep
int ksleep(int event)
{
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

//if we do not have a child we return -1
//if we do, we return 1 and save exit code in the status
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

int init()
{
   PROC *p;
   int i;

   /* initialize all proc's */
   printf("init...\n");
   for (i=0; i<NPROC; i++){
       p = &proc[i];
       p->pid = i;                        // pid = 0,1,2,..NPROC-1
       p->ppid = 0;
       p->status = FREE;
       p->priority = 0;
       p->next = &proc[i+1];              // point to next proc
   }
   
   proc[NPROC-1].next = 0;
   freeList = &proc[0];
   readyQueue = sleepList = 0;
   //create p0 as running
   p = get_proc(&freeList);
   p->ppid = 0;
   p->status = READY;
   running = p;
   nproc++;
   
   printf("init complete\n");
 }
   
int scheduler()
{
	if (running->status == READY)
	{
		enqueue(&readyQueue, running);
	}
	running = dequeue(&readyQueue);
}

main()
{
  printf("MTX starts in main()\n");
  init();
  kfork();
  while(1){
	if(readyQueue)
    {
		printf("TSWITCH()\n");
		tswitch();
	}
  }
}

int do_kfork()
{
	PROC *p = kfork();
	if(p == 0)
	{
		printf("kfork failed\n"); return -1;
	}
	printf("PROC %d kfork a child %d\n", running->pid, p->pid);
	return p->pid;
}

int do_tswitch()
{
	tswitch();
}

int do_wait()
{
	int pid, status;
	pid = kwait(&status);
	
	//if no children exist to wait on
	if(pid == -1)
	{
		printf("Process has no children for it to wait to become zombies!\n");
	}
	
	//if we do have children to wait on
	else
	{
		printf("pid=%d\tstatus=%d\n", pid, status);
	}
}

int do_exit()
{
	kexit(0);
}


