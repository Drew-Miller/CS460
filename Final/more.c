#include "ucode.c"
#include "cmd.c"

int main(int argc, char *argv[])
{
	int i = 0;
	char c[1];
	char input;
	
	//int to store the file descriptor
	//should be 0 if we are writing
	//stdin to stdout
	int fd = STDIN;
	
	//otherwise, if we have multiple arguments
	//passed in, the file can be opened to cat
	if(argc > 1)
	{
		fd = open(argv[1], O_RDONLY);
	}
	
	//if the file did NOT open properly
	//the return of fd is less than 0
	if(fd < 0)
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
		while(read(fd, c, 1))
		{
			if(i > ROWS)
			{
				input = getc();
				
				if(input == '\r') { i = ROWS - 1; }
				else if(input == ' ') { i = 0; }
				else if(input == '~') { putc('\r'); putc('\n'); exit(1); }
			}
		
			//cannot use print here.
			//must use putc BECAUSE
			//printf allows only output to
			//sh. PUTC writes to the tty structure
			//allowing us to pipe the output if
			//needed.
			putc(c[0]);
		
			if(c[0] == '\n' && argc > 1)
			{
				putc('\r');
			}
						
			if(c[0] == '\n' || c[0] == '\r')
			{
				i++;
			}
		}
	}
	
	//THIS IS FOR STDIN
	else
	{		
		while(read(fd, c, 1))
		{
			//cannot use print here.
			//must use putc BECAUSE
			//printf allows only output to
			//sh. PUTC writes to the tty structure
			//allowing us to pipe the output if
			//needed.		
			switch(c[0])
			{
				case '\r': putc('\n'); 	break;
				case '~': exit(1);		break;
			}
			
			putc(c[0]);
			
			if(c[0] == '\n' || c[0] == '\r')
			{
				i++;
			}
		}
	}
	
	putc('\n');
	close(fd);
	
	return 0;
}
