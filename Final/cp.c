#include "ucode.c"
#include "miller.h"

int main(int argc, char *argv[])
{
	int f1 = STDIN;
	int f2 = STDOUT;
	
	int r, i;
	char c[0];
		
		
	if(argc != 3)
	{
		printf("Invalid parameter amount.\n");
		return -1;
	}
	
	f1 = open(argv[1], O_RDONLY);
	f2 = open(argv[2], O_WRONLY | O_CREAT);
	
	if(f1 < 0 || f2 < 0)
	{
		printf("File(s) not opened correctly.\nExiting...\n");
		return -1;
	}
	
	
	//read all input from file
	while(read(f1, c, 1))
	{
		write(f2, c, 1);
	}
	
	putc('\n');
	close(f1);
	close(f2);
	
	return 0;
}
