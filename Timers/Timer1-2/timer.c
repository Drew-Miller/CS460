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
 *		    timer interrupt handler       		     *
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
    
    set_time(h, m, s);    
    
    //dec slice
    if(running->inkmode == 1)
    {
		running->time--;

		printf("P%d :time=%d |  ", running->pid, running->time);
		  
		if(running->time == 0)
		{
			tswitch();
		}	  
	}
	
  }
}
