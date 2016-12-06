#include "ucode.c"
#include "miller.h"

char *tty;
char uname[STD_BUF], pw[STD_BUF], home[STD_BUF], prog[STD_BUF];
int in, out, err;
int uid, gid;

main(int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{
  tty =  argv[1];

  close(0); close(1); close(2); // login process may run on different terms
  
  in = open(tty, O_RDONLY);
  out = open(tty, O_WRONLY);
  err = open(tty, O_WRONLY);

  // open its own tty as stdin, stdout, stderr

  settty(tty);   // store tty string in PROC.tty[] for putc()

  // NOW we can use printf, which calls putc() to our tty
  printf("MILLER-LOGIN : open %s as stdin, stdout, stderr\n", tty);

  signal(2,1);  // ignore Control-C interrupts so that 
                // Control-C KILLs other procs on this tty but not the main sh

  while(1){
	//get username and pswd from user
    printf("login: ");
    gets(uname);
    
    printf("password: ");
    gets(pw);
    
    //check if valid inputs using
    //the etc/passwd file
    if(checkUser() == 1)
    {
		printf("Loading User...\n");

		chuid(uid, gid);
		chdir(home);
			
		printf("\nWelcome to MILLER-MTX\nYou are logged in as: %s\n", uname);
		
		//need to trim prog
		trimProg();
		
		strcat(prog, " ");
		strcat(prog, uname);
		
		exec(prog);
	}
	
	else
	{
		printf("Username or Password Incorrect.\n");
	}
  }
}

//function for trimming the remaining
//characters from the program name
void trimProg()
{
	int i = 0;
	
	
	while(isAlpha(prog[i]))
	{ i++; }
	
	prog[i] = '\0';
}

//returns 1 if alpha, 0 otherwise
int isAlpha(char c)
{
	if(c >= 'A' && c <= 'Z') {return 1; }
	if(c <= 'z' && c >= 'a') {return 1; }
	
	return 0;
}

int checkUser()
{
	char line[256];
	char *tokens;
	int correct = 0;
	int i = 0;

	//open the file for reading
	int userFile = open("/etc/passwd", O_RDONLY);
		
	//constraint of only 100 users
	//allowed in the file.
	while(correct == 0 && i < 100)
	{	
		oneLine(userFile, line, 256);	
		
		//if we have read all of the lines, we should just return
		//we did not have a user
		if(line[0] == 0) { close(userFile); return 0; }
		
		//file has format of
		// username:password:gid:uid:fullname:HOMEDIR:program <- be careful of new lines
		// root:xxxxxxx:1000:0:superuser:/root:sh
		//first token is username
		//second is password.
		//we check these two for valid inputs from user
		tokens = strtok(line, ":");
	
		//if the username matches the given username
		//given from the user	
		if(strcmp(uname, tokens) == 0)
		{
			//get the password
			//this returns the next part of the line
			//which would be the password
			tokens = strtok(NULL, ":");
		
			if(strcmp(pw, tokens) == 0)
			{
				correct = 1;
				
				//get and convert
				//the gid and uid
				gid = atoi(strtok(NULL, ":"));	
				uid = atoi(strtok(NULL, ":"));
				
				//get the name of the user
				//the home directory, as well as
				//the program to execute
				strcpy(uname, strtok(NULL, ":"));
				strcpy(home, strtok(NULL, ":"));
				strcpy(prog, strtok(NULL, ":"));
			
			}
		}
		
		i++;
	}
	
	close(userFile);
	return correct;
}

//reads only one line from file.
void oneLine(int file, char* line, int maxLength)
{
	char single[1];
	int i = 0;
	
	while(1)
	{
		//read a single character from a file
		read(file, single, 1);
		
		line[i] = single[0];
				
		//once we have read a whole line,
		//we can stop this looping
		if(single[0] == '\n')
		{
			line[i] = '\0';
			break;
		}
		
		//if we have reached the maxlength
		//of the input buffer, we cannot continue
		if(i >= maxLength)
		{
			break;
		}
		
		i++;
	}
}

