#include "miller.h"

int stdin, stdout;

int findCmd(char *cmd)
{
	int i = 0;
		
	for(; i<(sizeof(commands)/sizeof(commands[0])); i++)
	{
		if(strcmp(cmd, commands[i]) == 0)
		{
			return i;
		}
	}
	
	return -1;
}

int logout(int val)
{
	printf("terminating process...\n");
	exit(val);
}

int cd(char *dir)
{
	chdir(dir);
}

//returns -1 if cannot read like at EOF
int getLine(int fd, char *buf, int len)
{
	char c = 0;
	int i = 0;
	char b[1];
	int l = -1;
	
	b[0] = 0;
	
	while(read(fd, b, 1) && l < len)
	{
		if(l < 0)
		{
			l = 0;
		}
		
		c = b[0];
		
		//if we read a null character
		if(c == '\r' || c == '\n')
		{
			buf[l] = 0;
			return l;
		}
		
		buf[l] = c;
		
		l++;
	}
	
	return l;
}

int splitCmd(char **arr, char *cmd)
{
	char cmdCpy[CMD_BUF];
	char *tmp;
	int i = 0;
	
	strcpy(cmdCpy, cmd);
			
	tmp = strtok(cmdCpy, " ");

	while(tmp != 0)
	{  
		arr[i++] = tmp;
		tmp =  strtok(0, " ");
	} 
		
	arr[i] = 0;
}

char *combine(char **args)
{
	char s[CMD_BUF];
	int i = 0;
	
	strcpy(s, "");
				
	while(strcmp(args[i],  0) != 0)
	{
		strcat(s, args[i++]);
		
		if(args[i])
		{
			strcat(s, " ");
		}
	}
		
	return s;
}
