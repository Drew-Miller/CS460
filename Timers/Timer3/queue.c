//inserts a proc into queue with
//highest priority at the front
int enqueue(PROC **queue, PROC *p)
{
    PROC *current, *next;

	//if the queue has no contents, we enqueue p
    if ((*queue) == 0)
    {
        *queue = p;
        (*queue)->next = 0;
    }
    
    //since we assume that the queue is already in priority order,
    //if p is greater than the first value, then we set p to the new
    //front
    else if (p->priority > (*queue)->priority)
    {
		//set p's next to the queue
        p->next = (*queue);
        //point the queue back to p
        (*queue) = p;
    }
    
    //if the p needs to be inserted somewhere
    //withtin the queue.
    else
    {
		//save the front as current
        current = (*queue);
        //save the next node as next
        next = current->next;
        
        //while the current node is not null and we found
        //a next node with a higher priority, we keep moving down the list
        while (next != 0 && (p->priority <= next->priority))
        {
			//we set our current node to the next
			//and next to the new next
            current = next;
            next = next->next;
        }
        
        //find a spot where our priority is higher than
        //the current priority, we insert our p there.
        current->next = p;
        p->next = next;
    }
}

PROC *dequeue(PROC **queue)
{
	//store the front of the queue for dequeuing
    PROC *p  = 0;
    if (*queue != 0)
    {
		 //set the new front of the queue to the next node
		 p = *queue;
		 *queue = (*queue)->next;
	}
	//return the old front of the queue
    return p;
}

//removes proc from list
PROC *get_proc(PROC **list)
{
	PROC *temp = 0;
	//if list has contents
	if (*list != 0)
	{
		//we return the first node of the list
		temp = *list;
		*list = temp->next;
		return temp;
	}
	
	//else null
	return 0;
}

//puts proc back into list
int put_proc(PROC **list, PROC *p)
{
	//if list has contents
	if (*list == 0)
	{
		//we enqueue the first node of the list
		*list = p;	
		(*list)->next = 0;
	}

	else
	{
		//we set the p's next pointer to queues next
		p->next = *list;
		//and set front of list to p
		*list = p;
	}
}

//prints a list
int printList(char *name, PROC *list)
{
	//temp proc
	PROC *prc;

	//pirnt name variable
	printf("%s  ", name);
	
	//if empty list
	if(list == 0)
	{
		printf("There are no procs in this list\n");
		return;
	}
	
	//else set prc to front of the list
	prc = list;
	//print the id of the proc
	printf("%d[%d]  ", prc->pid, prc->ppid);
	prc= prc->next;
	
	//while we are not at the end of the list
	while(prc != 0)
	{
		//print the id
		printf("--> %d[%d]  ", prc->pid, prc->ppid);
		prc= prc->next;
	}
	
	//when we reach the end
	printf("--> NULL\n");
}
