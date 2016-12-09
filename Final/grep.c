#include "ucode.c"
#include "cmd.c"

int main(int argc, char *argv[])
{
	char c[1];
	
	//1 is for redirected in
	//0 is for reg in
	int in = 0;
	int n = 0;
	int i = 0;
	int red;
	char pattern[STD_BUF];
	char inBuf[LARGE_BUF];
	int passOver = 1;
	
	//otherwise, if we have multiple arguments
	//passed in, the file can be opened to cat
	if(argc < 2)
	{
		printf("Grep Pattern failed.\ngrep pattern filename\n");
		exit(1);
	}
	
	if(argc > 2)
	{
		in = 1;
		close(0);
		stdin = open(argv[2], O_RDONLY);
	}
	
	if(stdin < 0) { printf("File failed to open\n"); exit(1); }
	
	red = isRedirect();
	
	strcpy(inBuf, "");
	
	strcpy(pattern, argv[1]);
	
	while((n = getLine(stdin, inBuf, LARGE_BUF)) && passOver)
	{	
		if(n == 0) { passOver = 0; }
		else { passOver = 1; }
		
		if(strstr(inBuf, pattern))
		{
			for(i = 0; i < n && i < LARGE_BUF; i++)
			{
				writeOver(inBuf[i], in, red);
			}
			
			writeOver('\n', in, red);
		}
	}
		
	exit(1);
}
