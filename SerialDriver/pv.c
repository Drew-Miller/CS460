void block(struct semaphore *s)
{
	running->status = BLOCK;
	enqueue(&s->queue, running);
	tswitch();
}

void signal(struct semaphore *s)
{
	PROC *p;
	p = dequeue(&s->queue);
	p->status = READY;
	enqueue(&readyQueue, p);
}

int P(struct semaphore *s)
{
  s->value--;
  if(s->value < 0)
  {
	  block(s);
  }
}

int V(struct semaphore *s)
{
	s->value++;
	if(s->value <= 0)
	{
		signal(s);
	}
}
