#include "ucode.c"
#include "cmd.c"

void writeOver(char c);
void writeConsole(char c);
void writeOut(char c);

int main(int argc, char *argv[])
{
	char c[1];
	printf("STDOUT:%d\nSTDIN:%d\n", stdout, stdin);
	
	//otherwise, if we have multiple arguments
	//passed in, the file can be opened to cat
	if(argc > 1)
	{
		stdin = open(argv[1], O_RDONLY);
	}
	
	//if the file did NOT open properly
	//the return of fd is less than 0
	if(stdin < 0)
	{
		printf("FILE did not open properly.\n");
		return -1;
	}
			
	while(read(stdin, c, 1))
	{
		if(c[0] == '~')
		{	
			writeConsole('\r');
			exit(1);	
			break;
		}

		writeOver(c[0]);
	}
	
	close(stdin);
	
	return 0;
}

void writeOver(char c)
{			
	//writing to stdout
	if(stdout == STDOUT)
	{
		writeConsole(c);
	}
	
	//writing to file
	else
	{	
		if(stdin == STDIN)
		{
			writeConsole(c);
		}

		writeOut(c);
	}
}

//use this for writing to out
void writeConsole(char c)
{
	char arr[1];
	arr[0] = c;
		
	if(c == '\r' && stdin == STDIN)
	{
		write(stdout, "\r\n", 2);
	}
	
	else if(c == '\n' && stdin != STDIN)
	{
		write(stdout, "\r\n", 2);
	}
	
	else
	{
		write(stdout, arr, 1);	
	}
}

void writeOut(char c)
{
	char arr[1];
	arr[0] = c;
	
	if(c == '\r' && stdin == STDIN)
	{
		putc('\n');
	}
	
	else
	{
		putc(c);
	}
}
