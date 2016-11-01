#include "ucode.c"

int color;

main(int argc, char *argv[])
{ 
  char name[64]; int pid, cmd, cs, i, j;
  
  printf("In the English image\n");
  
   printf("ARGC:%d\n", argc);
  
  for(i = 0; i < argc; i++) {printf("argv[%d ] = %s\n", i, argv[i]); }
  
         


  while(1){
    color = 0x0C;
    pid = getpid();
	cs = getcs();
    
	printf("----------------------------------------------\n");
	printf("I am proc %d in U mode: running segment=%x\n",pid, cs);
	show_menu();
	printf("Command ? ");
   
    //is in kernel here
    gets(name);
     
    if (name[0]==0) 
    {
		printf("Empty Command\n");
        continue;
	}

    cmd = find_cmd(name);
    switch(cmd){
           case 1 : ps();       		break;
           case 2 : chname();   		break;
           case 3 : ufork();    		break;
           case 4 : uswitch();  		break;
           case 5 : wait();     		break;
           case 6 : exit();     		break;
           case 7 : kmode();    		break;
           case 8 : uexec();
           
           case 9: 	pipe();				break;
           case 10: pfd();				break;
		   case 11: read_pipe();		break;
		   case 12: write_pipe();		break;
		   case 13: close_pipe();		break;
		   case 14: itimer();			break;

           default: invalid(name); 		break;
    }
  }
}
