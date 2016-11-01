/******************** timer.c file *************************************/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

u16 tick;
u16 seconds;

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));
}

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */

  printf("timer init\n");
  tick = 0; 
  seconds = 0;
  out_byte(TIMER_MODE, SQUARE_WAVE);	// set timer to run continuously
  out_byte(TIMER0, TIMER_COUNT);	// timer count low byte
  out_byte(TIMER0, TIMER_COUNT >> 8);	// timer count high byte 
  enable_irq(TIMER_IRQ); 
  
  set_time(0, 0, 0);
}

/*===================================================================*
 *		    timer interrupt handler       		     				 *
 *===================================================================*/
int thandler()
{
  u16 h = 0;
  u16 m = 0;
  u16 s = 0;
  u16 remainder = 0;
  
  out_byte(0x20, 0x20);                // tell 8259 PIC EOI
  
  tick++; 
  tick %= 60;
  if (tick == 0)
  {                      // at each second      
    seconds++;
    
    //get h m and s from seconds
    h = (seconds / (60 * 60)) % 100;
    remainder = seconds - h * (60 * 60);
    m = (remainder / 60);
    remainder = remainder - m * 60;
	s = remainder;
    
    //check if inkmode is = 1;
    if(tq != 0)
    {
		printf("PID:%d  time:%d\n", tq->p->pid, tq->time);
		tq->time--;
		
		if(tq->time == 0)
		{
			//dequeue the tq
			tdequeue();
		}
	}    
	
	set_time(h, m, s);
  }
}

int tenqueue(TQE *t)
{		
	TQE *prev, *cur;
		
	//if the queue is empty
	if(tq == 0)
	{
		tq = t;
		tq->next = 0;
	}
		
	//else enqueue the new tq
	else
	{
		cur = tq;
		prev = 0;
		
		//while we do not have a null current node
		//and our time is less than the t time
		while(cur != 0 && cur->time < t->time)
		{			
			//decrement time so it is all relative
			t->time = t->time - cur->time;
			
			//move the pointer forward
			prev = cur;
			cur = cur->next;
		}
		
		//enqueue t to previous
		if(prev != 0)
		{
			prev->next = t;
		}
		
		//else, we did not have a prev, meaning add it to the head
		else
		{
			tq = t;
		}
		
		t->next = cur;
		
		if(cur != 0)
		{
			cur->time = cur->time - t->time;
		}
	}
	
	cur = tq;
	
	while(cur != 0)
	{
		cur = cur->next;
	}
	
	ksleep(t->p->pid);

	return 0;
}

int tdequeue()
{
	
	//wakeup here
	printf("DEQUEUE AND WAKEUP P:%d\n", tq->p->pid);
	kwakeup(tq->p->pid);
	tq = tq->next;
}
