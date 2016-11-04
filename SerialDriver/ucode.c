// ucode.c file


char *cmd[]={"pid", "ps", "chname", "fork", "switch", "wait", "exit", "kmode", "exec",
				"pipe", "pfd", "read", "write", "close","itimer","sin", "sout", 0};
int pd[2];
int getc();
void putc(char c);
#define LEN 64

int show_menu()
{
   printf("***************** Menu ****************************\n");
   printf("*  ps kmode chname  fork  switch  wait exec  exit *\n");
   printf("*  pipe  pfd  read  write  close  itimer          *\n");
   printf("*  sin   sout                                     *\n");
   printf("***************************************************\n");
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

int kmode()
{
	int r;
	printf("P[%d] going to kernel\n", getpid());
	r = syscall(3,0,0);
	printf("P[%d] returned from kernel!\n", getpid());
	return r;
}

int uswitch()
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

int ufork()
{   
  int child = fork();
  (child) ? printf("parent ") : printf("child ");
  printf("%d return from fork, child_pid=%d\n", getpid(), child);
}    

int fork()
{
	return syscall(7,0,0);
}

int uexec()
{
  int r;
  char filename[32];
  printf("enter exec filename : ");
  gets(filename);
  r = exec(filename);
  printf("exec failed\n");
}

int exec(char *s)
{
	return syscall(8,s,0,0);
}

//pipe syscalls
int pipe()
{
   printf("pipe syscall\n");
   
   syscall(30, pd, 0);
   printf("proc %d created a pipe with fd = %d %d\n", 
			getpid(), pd[0], pd[1]);
}

int read_pipe()
{
  char fds[32], buf[1024]; 
  int fd, n, nbytes;
  pfd();

  printf("read : enter fd nbytes : ");
  gets(fds);
  sscanf(fds, "%d %d",&fd, &nbytes);
  printf("fd=%d  nbytes=%d\n", fd, nbytes); 

  n = syscall(31, fd, buf, nbytes);

  if (n>=0){
     printf("proc %d back to Umode, read %d bytes from pipe : ",
             getpid(), n);
     buf[n]=0;
     printf("%s\n", buf);
  }
  else
    printf("read pipe failed\n");
}

int write_pipe()
{
  char fds[16], buf[1024]; 
  int fd, n, nbytes;
  pfd();
  printf("write : enter fd text : ");
  gets(fds);
  sscanf(fds, "%d %s", &fd, buf);
  nbytes = strlen(buf);
            
  printf("fd=%d nbytes=%d : %s\n", fd, nbytes, buf);

  n = syscall(32,fd,buf,nbytes);

  if (n>=0)
  {
     printf("\nproc %d back to Umode, wrote %d bytes to pipe\n", getpid(),n);
     uswitch();
  }
  else
  {
    printf("write pipe failed\n");
  }
}

int close_pipe()
{
  char s[16]; 
  int fd;
  printf("enter fd to close : ");
  gets(s);
  fd = atoi(s);
  syscall(33, fd);
}

int pfd()
{
  syscall(34,0,0,0);
}

//syscalls for putc and getc
int getc()
{
	syscall(90,0,0);
}

void putc(char c)
{
	syscall(91,c,0);
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

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}

int itimer()
{
	int t;
	char s[100];
	printf("Please enter a time for the timer to sleep: ");
	gets(s);
	t = myAtoi(s);
	syscall(35, t, 0);
	return t;
}

char line[64];
int sin()
{
	syscall(10,line, 0);
	printf("line=%d\n", line);
}

int sout()
{
	char line[64];
	
	//printf("Enter line to send to serial: ");
	//gets(line);
	syscall(9, "serial line from umode\r\n");
}
