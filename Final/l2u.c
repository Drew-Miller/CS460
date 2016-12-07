#include "ucode.c"
#include "cmd.c"

int main(int argc, char *argv[])
{
	char c[1];
	
	//1 is for redirected in
	//0 is for reg in
	int red;
	int in = 0;
	
	int stdin, fd = 0;
	
	//otherwise, if we have multiple arguments
	//passed in, the file can be opened to cat
	if(argc == 2)
	{
		in = 1;
		close(0);
		stdin = open(argv[1], O_RDONLY);
	}
	
	else if(argc == 3)
	{
		in = 1;
		close(0);
		close(1);
		stdin = open(argv[1], O_RDONLY);
		fd = open(argv[2], O_WRONLY | O_CREAT);
	}
	
	if(stdin < 0 || fd < 0)
	{
		printf("File(s) not opened correctly.\nExiting...\n");
		return -1;
	}
		
	red = isRedirect();	
	
	while(read(stdin, c, 1))
	{
		if(c[0] == '~')
		{
			writeOver('\r', in, red);
			close(STDIN);
			exit(0);
		}
		
		writeOver(convert(c[0]), in, red);
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
