#include "ucode.c"

int color;


main(int argc, char *argv[])
{ 
  char name[64]; int pid, cmd, cs, i;
  
  printf("En la imagen Espanola\n");
  
  printf("ARGC:%d\n", argc);
  
  for(i = 0; i < argc; i++) {printf("argv[%d ] = %s\n", i, argv[i]); }

  while(1){
    color = 0x0C;
    pid = getpid();
	cs = getcs();
       
    printf("-----------------------------------\n");
    printf("Soy proceso %d en el modo de usario :\ncorriendo segmento =%x\n",pid, cs);
    show_menu();
    printf("Commando ? ");
    gets(name);
     
    if (name[0]==0) 
    {
		printf("Entrado Nada\n");
        continue;
	}

    cmd = find_cmd(name);
    switch(cmd){
           case 1 : ps();       break;
           case 2 : chname();   break;
           case 3 : ufork();    break;
           case 4 : uswitch();  break;
           case 5 : wait();     break;
           case 6 : exit();     break;
           case 7 : kmode();    break;
           case 8 : uexec();

           default: invalid(name); break;
    }
  }
}
