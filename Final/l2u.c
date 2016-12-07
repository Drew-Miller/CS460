#include "ucode.c"
#include "cmd.c"

int main(int argc, char *argv[])
{
	char c[1];
	
	//1 is for redirected in
	//0 is for reg in
	int in = 0;
	int red = isRedirect();
	
	int f1, f2 = 0;
	
	//otherwise, if we have multiple arguments
	//passed in, the file can be opened to cat
	if(argc == 2)
	{
		in = 1;
		f1 = open(argv[1], O_RDONLY);
	}
	
	else if(argc == 3)
	{
		in = 1;
		f1 = open(argv[1], O_RDONLY);
		f2 = open(argv[2], O_WRONLY | O_CREAT);
	}
	
	if(f1 < 0 || f2 < 0)
	{
		printf("File(s) not opened correctly.\nExiting...\n");
		return -1;
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

char convert(char input)
{
	if(input >= 'a' && input <= 'z')
	{
		return input - 32;
	}
	
	return input;
}
