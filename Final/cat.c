#include "ucode.c"
#include "cmd.c"

int main(int argc, char *argv[])
{
	char c[1];
	
	//int to store the file descriptor
	//should be 0 if we are writing
	//stdin to stdout
	int fd = 0;
		
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
		
	printf("----MILLER CAT----\n");
	printf("     WOOF!        \n\n");
	
	//read returns amount of bytes read.
	//as long as the value is not 0, we loop
	//the read function.
	//THIS IS FOR A FILE
	if(argc > 1)
	{
		while(read(fd, c, 1))
		{
			putc(c[0]);

			//cannot use print here.
			//must use putc BECAUSE
			//printf allows only output to
			//sh. PUTC writes to the tty structure
			//allowing us to pipe the output if
			//needed.		
			if(c[0] == '\n')
			{
				putc('\r');
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
				case '~': putc('\n'); exit(1);		break;
			}
			
			putc(c[0]);
		}
	}
	
	putc('\n');
	close(fd);
	
	return 0;
}
