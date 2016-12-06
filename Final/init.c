#include "ucode.c"
#include "cmd.c"

int pid, child[CHILD_AMOUNT];

main(int argc, char *argv[])
{
	// open /dev/tty0 as 0 (READ) and 1 (WRTIE) in order to display messages

	// Now we can use printf, which calls putc(), which writes to stdout
		
	int i = 0;
	
	open("/dev/tty0", O_RDONLY);
	open("/dev/tty0", O_WRONLY);
	 
    printf("MILLER-INIT : fork a login task on console\n"); 
    
    for(i = 0; i < CHILD_AMOUNT; i++)
    {
		child[i] = fork();
		
		if(!child[i]) { login(i); }
	}
	
	parent();
}       

//log in function that takes
//essentially the index of the child
//and executes the correct log in
int login(int i)
{
	switch(i)
	{
		case 0:
			exec("login /dev/ttyS0");
			break;
		case 1:
			exec("login /dev/ttyS1"); 
			break;
		case 2:
			exec("login /dev/tty0"); 
			break;
	}
}
      
int parent()
{
	int i, index;
	int isChild = 0;
	int status;
	
    while(1){
		printf("\nMILLER-INIT : parent waiting .....\n");

		pid = wait(&status);

		for(i = 0; i < CHILD_AMOUNT; i++)
		{
			//if the process that died is one
			//of the original children
			if(pid == child[i])
			{
				//we mark that it is a child	
				isChild = 1;
				index = i;
			}
		}
		
		//if we have marked that a child has died
		if(isChild)
		{
			//we fork a new process
			//and login on the child
			child[index] = fork();
			
			
			if(!child[index])
				login(index);
		}

		else
		{
			printf("INIT : buried an orphan child %d\n", pid);
		}
	}
}
