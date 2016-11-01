#define MAXLEN 128


//default base and string to get char of the ints
char *ctable = "0123456789ABCDEF";
u16 BASE = 10;

//recursively prints integers based on the base
int rpu(u16 x)
{
	char c;
	if(x)
	{
		c = ctable[x % BASE];
		rpu(x / BASE);
		putc(c);
	}
}

//16 bit signed
int printd(u16 x)
{
	//max value of short
	int max = 32767;
	
	//if negative,
	//set the u16 to its complement
	if(x > max)
	{
		putc('-');
		x = ~x + 1;
	}
	
	//make sure base is still 10, even though it should be
	//by default
	BASE = 10;
	(x==0) ? putc('0') : rpu(x);
	putc(' ');
	BASE = 10;
}

//16 bit unsigned
int printu(u16 x)
{
	BASE = 10;
	(x==0) ? putc('0') : rpu(x);
	putc(' ');
	BASE = 10;
}

//32 bit long
int printl(u32 x)
{	
	BASE = 10;
	(x==0) ? putc('0') : rpu(x);
	putc(' ');
	BASE = 10;
}

//16 bit hex
int printx(u16 x)
{
	BASE = 16;
	(x==0) ? putc('0') : rpu(x);
	putc(' ');
	BASE = 10;
}

//32 bit hex
int printX(u32 x)
{
	BASE = 16;
	(x==0) ? putc('0') : rpu(x);
	putc(' ');
	BASE = 10;
}

char prints(char *s)
{
	while(*s != '\0')
	{
		putc(*s);
		s++;
	}
}

//gets a string input from keyboard
char *gets(char s[MAXLEN])
{
	char c, *t = s;
	int len = 0;
	
	while((c = getc()) != '\r' && (len < MAXLEN - 1))
	{
		*t++ = c;
		putc(c);
		len++;
	}
	
	*t = 0;
	return s;
}

//printing function
int printf(char *fmt)
{
	char *cp	=	fmt;
	u16 *ip		=	(u16 *)&fmt + 1;
	u32 *up;
	
	while(*cp)
	{
		if(*cp != '%')
		{
			putc(*cp);
			if(*cp == '\n')
			{
				putc('\r');
			}
			cp++;
			continue;
		}
		
		cp++;
		switch(*cp)
		{
			case 'c' : putc(*ip); break;
			case 's' : prints(*ip); break;
			case 'u' : printu(*ip); break;
			case 'd' : printd(*ip); break;
			case 'x' : printx(*ip); break;
			case 'l' : printl(*ip); break;
			case 'X' : printX(*ip); break;
		}
		
		cp++;
		ip++;
	}
}
