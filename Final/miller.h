#define CHILD_AMOUNT 	3
#define STD_BUF 		64
#define LARGE_BUF 		256
#define CMD_BUF 		128

#define ROWS 			20
#define COLUMNS 		80

#define STDIN 			0
#define STDOUT 			1

int checkUser();
int isAlpha();
void trimProg();
void oneLine(int file, char line[], int maxLength);

int login(int i);
int parent();

int runCmd();

char convert(char input);

int executeInput(char *args[]);

int splitCmd(char *arr[], char *cmd);
char *combine(char *args[]);
int redirect(char *args[]);

char *commands[] = {"exit", "logout", "cd", "ls", "cat", "grep", "cp", "l2u", "more", "rm"};
