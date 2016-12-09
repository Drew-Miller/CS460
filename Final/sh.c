#include "ucode.c"
#include "cmd.c"

char currentDir[STD_BUF];
char input[CMD_BUF];
char cmd[CMD_BUF];

main(int argc, char *argv[ ])
{		
	while(1)
	{
		getcwd(currentDir);
		
		printf("%s$ ", currentDir);
		gets(input);
		
		runCmd();
	}
}

//if logout, cd, or su
//run command directly here
int runCmd()
{
	int pid;
	int pp;
	int status;
	int cmdIndex;
	char *args[STD_BUF];
	char cmdCpy[CMD_BUF];
	char *token = 0;
	int i = 0;	
	
	strcpy(cmdCpy, input);
	splitCmd(args, cmdCpy);
	
	cmdIndex = findCmd(args[0]);
				
	if(cmdIndex == 0 || cmdIndex == 1)
	{
		logout(0);
		return;
	}
	
	else if(cmdIndex == 2)
	{
		chdir(args[1]);
		return;
	}

	else if(cmdIndex >= 0)
	{		
		pid = fork();
		
		//papa process
		if(pid)
		{
			pid = wait(&status);
		}
		
		//child process
		else
		{	
			pp = hasPipe(args);
			
			if(pp < 0)
			{	
				executeInput(args);	
				exit(0);
			}
			
			else
			{
				i = 0;
				
				executePipe(args);
			}
		}
	}
}

int hasPipe(char *args[])
{
	int i = 0;
	while(strcmp(args[i], 0) != 0)
	{
		if(strcmp(args[i], "|") == 0)
		{
			return i;
		}
		
		i++;
	}
	
	return -1;
}

int executePipe(char *args[])
{
	int pd[2];
	int i = 0;
	int j = 0;
	int pid;
	char head[CMD_BUF];
	char tail[CMD_BUF];
	char combined[CMD_BUF];
	char *token;
	char *hBuf[STD_BUF];
	char *tBuf[STD_BUF];
			
	i = 0;
	strcpy(combined, combine(args));
	
	//get head	
	token = strtok(combined, "|");
	strcpy(head, trimN(token));
	
	//get tail	
	token = strtok(0, "");
	strcpy(tail, trimN(token));
	
	pipe(pd);
	
	pid = fork();
	
	//PAPA is head
	if(pid)
	{
		splitCmd(hBuf, head);
		
		//set up the head pipe
		//close stdout, use our pipe for out
		//close end of pipe
		close(pd[0]);
		close(1);
		dup2(pd[1], 1);
		
		executeInput(hBuf);
	}
	
	else
	{
		splitCmd(tBuf, tail);
		
		//set up the tail pipe
		//close stdin, use our pipe for out
		//close end of pipe
		close(pd[1]);
		close(0);
		dup2(pd[0], 0);

		//if the tail has a pipe as well
		if(hasPipe(tBuf) > -1)
		{
			executePipe(tBuf);
		}
		
		//if we do not have a pipe
		//just execute it
		else
		{
			//close(1);
			//open("/dev/tty0", O_WRONLY);
			
			executeInput(tBuf);
		}
		
		exit(1);
	}
}

int executeInput(char **args)
{
	int i, n = 0;
		
	n = redirect(args);
	
	if(n != -1)
	{
		exec(redirectCombine(args, n));
	}
	
	else
	{
		exec(combine(args));
	}
}

int redirect(char **args)
{
	int i = 1;
		
	if(strcmp(args[1], 0) == 0) { return -1; }
	
	while(strcmp(args[i], 0) != 0)
	{
		if(strcmp(args[i], "<") == 0)
		{
			close(0);
			open(args[i+1], O_RDONLY);
			return i;
		}
	
		else if(strcmp(args[i], ">") == 0)
		{
			close(1);
			open(args[i+1], O_WRONLY|O_CREAT);
			return i;
		}
	
		else if(strcmp(args[i], ">>") == 0)
		{
			close(1);
			open(args[i+1], O_APPEND|O_WRONLY|O_CREAT);	
			return i;
		}
		
		i++;
	}
	
	return -1;
}
