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
			executeInput(args);	
			exit(0);
		}
	}
}

int executeInput(char **args)
{
	int i, n = 0;
		
	if(n = redirect(args))
	{
		exec(args[0]);
	}
	
	else
	{
		exec(combine(args));
	}
}

int redirect(char **args)
{
	if(strcmp(args[1], 0) == 0) { return 0; }
	
	if(strcmp(args[1], "<") == 0)
	{
		close(0);
		stdin = open(args[2], O_RDONLY);
		return 1;
	}
	
	else if(strcmp(args[1], ">") == 0)
	{
		close(1);
		stdout = open(args[2], O_WRONLY|O_CREAT);
		return 1;
	}
	
	else if(strcmp(args[1], ">>") == 0)
	{
		close(1);
		stdout = open(args[2], O_APPEND|O_WRONLY|O_CREAT);	
		return 1;
	}
	
	return 0;
}
