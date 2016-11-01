// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "exit", 0};

#define LEN 64

//syscalls for putc and getc
void putc(char c)
{
	syscall(8,c,0);
}

int getc()
{
	syscall(7,0,0);
}

int show_menu()
{
   printf("***************** Menu *******************\n");
   printf("*  ps  chname  kfork  switch  wait  exit *\n");
   printf("******************************************\n");
}

//finds index of command
int find_cmd(char *name)
{
	int i = 0; char *p = cmd[0];
	while(p)
	{	
		//if they match (strcmp() = 0), we return the index
		if(!strcmp(p, name))
		{
			return i;
		}
		
		i++;
		p=cmd[i];
	}
	
	//name not in function
	invalid(name);
	return -1;
}

int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   return syscall(1, 0, 0);
}

int chname()
{
    char s[32];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int kfork()
{   
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(3, 0, 0);
  if(child != -1){printf("proc %d kforked a child %d\n", pid, child);}
  else{printf("proc %d kfork FAILED\n", pid);}
}    

int kswitch()
{
    return syscall(4,0,0);
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
} 

//use syscall for getc and putc
int geti()
{
  char buf[32], c;
  int num, i;
  
  //set all values to null terminator
  for(i = 0; i<32;i++)
  {
	  buf[i] = '\0';
  }
  
  //get string from the kernel
  gets(buf);
  
  num = myAtoi(buf);
    
  return num;
}

//turns string to int. returns 0 if not a full int
int myAtoi(char *s)
{
	int result;
	int i;
	result = 0;
	i = 0;
		
	while(*s)
	{
		//if we have an integer number
		if(*s >= 8 && *s <= 57)
		{
			//we multiply the result by a factor of ten and add the
			//new value from the string to it
			result = result * 10 + (int)(s[i] - '0');
			s++;
		}
		
		//returns 0 if s is not all ascii integers
		else {return 0;}
	}
	
	return result;
}

//disallow exiting of p1!!
int exit()
{
   int exitValue;
   printf("enter an exitValue: ");
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
	
  return syscall(6,exitValue,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}
