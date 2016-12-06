#include "ucode.c"
#include "cmd.c"

int main(int argc, char *argv[])
{
	int f1 = STDIN;
	int f2 = STDOUT;
	
	int r, i;
	char c[0];
		
		
	//makes file capital
	if(argc == 2)
	{
		f1 = open(argv[1], O_RDONLY);
	}
		
	//case you need to write to a different file
	//can use open create to create a file if need be
	else if(argc == 3)
	{
		f1 = open(argv[1], O_RDONLY);
		f2 = open(argv[2], O_WRONLY | O_CREAT);
	}
	
	if(f1 < 0 || f2 < 0)
	{
		printf("File(s) not opened correctly.\nExiting...\n");
		return -1;
	}
	
	
	//read all input from file
	while(read(f1, c, 1))
	{
		if(f1 == STDIN && c[0] == '~') { break;}
		
		c[0] = convert(c[0]);
		
		//if we are writing to a file
		if(f2 != STDOUT)
		{
			write(f2, c, 1);
		}
		
		//if we are writing to stdout
		else
		{
			putc(c[0]);
			
			if(c[0] == '\r')
			{
				putc('\n');
			}					
		}
	}
	
	putc('\n');
	close(f1);
	close(f2);
	
	return 0;
}

char convert(char input)
{
	if(input >= 'a' && input <= 'z')
	{
		return input - 32;
	}
	
	return input;
}
