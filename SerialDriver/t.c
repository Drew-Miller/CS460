#include "type.h"
#define TIMESLICE 5

//fix exit so we cannot exit 1, just resumes kernel mode

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
TQE *tq, tqe[NPROC];	//timer queue. tq is pointing to the head

int procSize = sizeof(PROC);
int nproc = 0;

char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
               "Saturn", "Uranus", "Neptune" };
               
OFT  oft[NOFT];
PIPE pipe[NPIPE];

/**************************************************
  bio.o, queue.o loader.o are in mtxlib
**************************************************/
/* #include "bio.c" */
/* #include "queue.c" */
/* #include "loader.c" */

#include "pipe.c"
#include "queue.c"
#include "wait.c"
#include "kernel.c"           // YOUR kernel.c file
#include "int.c"              // YOUR int.c    file
#include "do_functions.c"
#include "serial.c"
#include "pv.c"

int color;

//prints a list
int printList(char *name, PROC *list);

int body()
{
   char c;
   printf("proc %d starts from body()\n", running->pid);
   while(1){
     color = running->pid + 9;
     
     printList("readyQueue", readyQueue);
     printList("freeList", freeList);
     
     printf("proc %d running : parent=%d\n", running->pid, running->priority,running->ppid);
     printf("enter a char [s|f|w|q|i|p|u]: ");
     c = getc(); printf("%c\n", c);
     
     switch(c)
     {
		case 'f': do_kfork(); break;
		case 's': do_tswitch(); break;
		case 'w': do_wait(); break;
		case 'q': do_exit(); break;
		case 'i': do_pid(); break;
		case 'p': do_ps(); break;
		case 'u': do_umode(); break;
 		default: printf("invalid command\n"); break;
	 }
   }
}

int init()
{
    PROC *p; int i, j;
    color = 0x0C;
    printf("init ....");
    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);
        //displays the proc that was created and its name
        printf("proc[%d]=%s.\n", i, proc[i].name);
        p->inkmode = 1;
        p->next = &proc[i+1];
        
        //set up fd to 0
        for(j = 0; j < NFD; j++)
        {
			p->fd[j] = 0;
		}
    }
    
    tq = 0;
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
} 

int scheduler()
{
    if (running->status == READY)
        enqueue(&readyQueue, running);
        
    
	running = dequeue(&readyQueue);
	color = running->pid + 0x0A;
}

int int80h(); int tinth(); int s0inth(); int s1inth();

int set_vector(u16 segment, u16 handler)
{
     // put_word(word, segment, offset)
     put_word(handler, 0, segment<<2);
     put_word(0x1000,  0,(segment<<2) + 2);
}
            
main()
{
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    set_vector(80, int80h);

	//only time this image needs to be called.
	//any other time the proc just copies this image from p1
    kfork("/bin/u1");     // P0 kfork() P1
    lock();

	set_vector(12, s0inth);
    sinit();

    while(1){
      printf("P0 running\n");
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();         // P0 switch to run P1
   }
}
