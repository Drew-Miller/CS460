#include "ucode.c"
#include "cmd.c"

int main(int argc, char *argv[])
{
	char c[1];
	
	//1 is for redirected in
	//0 is for reg in
	int in = 0;
	int red = isRedirect();
	
	//otherwise, if we have multiple arguments
	//passed in, the file can be opened to cat
	if(argc > 1)
	{
		in = 1;
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
			writeOver('\r', in, red);
			close(STDIN);
			exit(0);
		}
		
		writeOver(c[0], in, red);
	}
	
	exit(0);
}

