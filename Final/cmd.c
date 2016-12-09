#include "miller.h"
	
int stdin = STDIN;
int stdout = STDOUT;

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
	int l = 0;
	
	b[0] = 0;
	
	while(read(fd, b, 1) && l < len)
	{
		if(l < 0)
		{
			l = 0;
		}
		
		c = b[0];
				
		//if we read an enter character
		if(c == '\r' || c == '\n')
		{
			buf[l++] = 0;
			break;
		}
		
		buf[l] = c;
		
		l++;
	}
		
	return l;
}

int splitCmd(char *arr[], char *cmd)
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

char *redirectCombine(char **args, int n)
{
	char s[CMD_BUF];
	int i = 0;
	
	strcpy(s, "");
				
	while(i < n && strcmp(args[i], 0) != 0)
	{
		strcat(s, args[i++]);
		
		if(i + 1 <= n)
		{
			strcat(s, " ");
		}
	}
		
	return s;
}

void writeOver(char c, int in, int red)
{
	char arr[1];
	arr[0] = c;
	
	//if stdin is reg
	if(in == 0)
	{
		//if we are not redirected
		//write straight to console
		if(red == 0)
		{
			if(c == '\r')
			{
				write(2, "\r\n", 2);
			}
			
			
			//not a enter character
			else
			{
				write(2, arr, 1);
			}
		}
		
		//if we have a redirected output to a file
		else
		{
			//if we have an enter character
			if(c == '\r')
			{
				write(1, "\n", 1);
				write(2, "\r\n", 2);
			}
			
			else
			{
				write(1, arr, 1);
				write(2, arr, 1);
			}
		}
	}
	
	//if stdin in is a file
	else
	{
		//if we are not redirected
		//write straight to console
		if(red == 0)
		{
			if(c == '\n')
			{
				write(2, "\r\n", 2);
			}
			
			else
			{
				write(2, arr, 1);
			}
		}
		
		//if we are writing out to a file
		else
		{
			write(1, arr, 1);
		}
	}
}

//returns 1 if writing to file
//0 if not.
int isRedirect()
{
	STAT *s = (STAT*)malloc(sizeof(stat));

	//receive the stats from stdout
	fstat(STDOUT, s);

	//if the stat of STDOUT is a file
	if ((s->st_mode & 0xF000) == 0x8000 || (s->st_mode & 0xF000) == 0x4000 || (s->st_mode & 0xF000) == 0xA000) 
	{

		//we do have a redirect
		return 1;
	}

	return 0;
}

char *trimN(char *s)
{
	int i = 0;
	
	while(s[i] != '\0')
	{
		i++;
	}
	
	if(s[i - 1] == ' ') { s[i - 1] = 0; }
	if(s[0] == ' ') { s++; }
	
	return s;
}
