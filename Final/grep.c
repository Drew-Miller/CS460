#include "ucode.c"
#include "cmd.c"

int main(int argc, char *argv[])
{
	int fd = STDIN;
	int n = 0;
	char inBuf[LARGE_BUF];
	char pattern[STD_BUF];
	
	if(argc < 2)
	{
		printf("Grep failed.\ngrep pattern [filename]\n");
		return -1;
	}
	
	if(argc >= 3)
	{
		fd = open(argv[2], O_RDONLY);
	}
	
	if(fd < 0) { printf("File failed to open\n"); }
	
	strcpy(pattern, argv[1]);
		
	while(!strstr(inBuf, 26) && n >= 0)
	{
		strcpy(inBuf, "");
		
		n = getLine(fd, inBuf, LARGE_BUF);
		
		//to end early, use a tilde
		if(strstr(inBuf, "~")) { return 0; }
				 
		if(strstr(inBuf, pattern))
		{
			printf("%s\n", inBuf);
		}
	}
	
	return 0;
}
