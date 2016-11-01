typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define NPROC 9                // number of PROCs
#define SSIZE 1024             // per proc stack area 
#define NULL  0

#define FREE	0
#define READY	1
#define STOP	2
#define DEAD	3

typedef struct proc{
    struct proc *next;
    int    *ksp;               // saved ksp when not running
    int    pid;                // add pid for identify the proc
    int	   ppid;			   // parent pid
    int	   status;			   // status = FREE|READY|STOP|DEAD, etc
    int	   priority;		   // scheduling priority
    int    kstack[SSIZE];      // proc stack area
}PROC;


int  procSize = sizeof(PROC);
PROC proc[NPROC], *running, *freeList, *readyQueue;    // define NPROC procs
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

     printf("proc %d running : parent=%d\n", running->pid, running->ppid);
     printf("enter a char [s|f]: ");
     c = getc(); printf("%c\n", c);
     switch(c)
     {
		case 'f': do_kfork(); break;
		case 's': do_tswitch(); break;
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
	p->status = READY;
	p->priority = 1;
	p->ppid = running->pid;
	//initialize new proc's kstack
	for(i = 1; i < 10; i++)
	{
		p->kstack[SSIZE -1] = (int)body;
	}
	
	p->ksp = &p->kstack[SSIZE - 9];
	enqueue(&readyQueue, p);
	return p;
}

int init()
{
   PROC *p;
   int i, j;

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
   readyQueue = 0;
   //create p0 as running
   p = get_proc(&freeList);
   p->ppid = 0;
   p->status = READY;
   running = p;
   
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

