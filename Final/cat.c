#include "ucode.c"
#include "cmd.c"

int main(int argc, char *argv[])
{
	char c[1];
	
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
		
	//read returns amount of bytes read.
	//as long as the value is not 0, we loop
	//the read function.
	//THIS IS FOR A FILE
	if(argc > 1)
	{
		while(read(stdin, c, 1))
		{
			putc(c[0]);
			
			if(c[0] == '\n')
			{
				putc('\r');
			}
		}
	}
	
	//THIS IS FOR STDIN
	else
	{		
		while(read(stdin, c, 1))
		{
			//cannot use print here.
			//must use putc BECAUSE
			//printf allows only output to
			//sh. PUTC writes to the tty structure
			//allowing us to pipe the output if
			//needed.		
			switch(c[0])
			{
				case '\r': 
					putc('\n');
					write(2, "\n", 1);	
					break;
					
				case '~': 	
					write(2, "\n", 1);
					exit(1);	
					break;
			}
			
			write(2, c, 1);
			putc(c[0]);
		}
	}
	
	putc('\n');
	close(stdin);
	
	return 0;
}
