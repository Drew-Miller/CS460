#define VDC_INDEX		0x3D4
#define VDC_DATA		0x3D5
#define CUR_SIZE		10		//cursor size reg
#define VID_ORG			12		//start address register
#define CURSOR			14		//cursor position register
#define LINE_WIDTH		80		// number of characters in a line
#define SCL_LINES		25		// amount of lines on the screen
#define SCR_BYTES		4000	//bytes on the screen (width * lines)
#define CURSOR_SHAPE	15		//block cursor for EGA/VGA
//attribute byte: 0x0HRGB, H=highlight; RGB determines color
u16 base = 0xB800;				//VRAM Base Address
u16 vid_mask = 0x3FFF;			//mask = Video RAM size - 1
u16 offset;						//offset from VRAM segment base
int color;						//attribute byte
int org;						//current display origin, r.e. VRAM BASE
int row, column;				//logical row, col position

void remove_time();

int vid_init() // initializes org = 0 (row, column) = (0,0)
{
	int i, w;
	org = row = column = 0; 			//initialize globals
	color = 0x0A;						//high yellow
	
	set_VDC(CUR_SIZE, CURSOR_SHAPE);	//set cursor size
	set_VDC(VID_ORG, 0);				//display origin to 0
	set_VDC(CURSOR, 0);					//set cursor position to 0
	
	w = 0x0700;							//White, blank char
	for(i=0; i<25*80; i++)
	{
		put_word(w, base, 2*i);			//write 25*80 blanks to VRAM
	}
}

int scroll()	//scroll up one line
{
	u16 i, w, bytes;
	//test offset = org + ONE screen + ONE more line
	offset = org + SCR_BYTES + 2*LINE_WIDTH;
	
	remove_time();			//remove the time from the screen


	if(offset <= vid_mask)		//offset still within VRAM area
	{
		org += 2*LINE_WIDTH;		//just advance org by ONE line
	}
	
	else  						//offset exceeds VRAM area ==> reset to FRAM beginning by
								//copy current rows 1-24 to BASE, then reset org to 0
	{		
		for(i =0; i<24*80; i++)
		{
			//gets the origin from 2*80 for the rows that are just off the screen
			w = get_word(base, (org+160)+2*i);
			put_word(w, base, 2*i);
		}
		
		org = 0;
	}
	
	//org has been set up properly
	offset = org + 2*24*80;		//offset = beginning of row 24
	//copy a line of BLANKs to row 24
	w = 0x0C00;					//HRGB=1100==>HighList RED, Null Char
	
	for(i=0; i<80; i++)
	{
		put_word(w, base, offset + 2*i);
	}
	
	set_VDC(VID_ORG, org >> 1);		// set VID_ORG to org
}

int move_cursor()	//move cursor to current position
{
	int pos = 2*(row*80 + column);
	offset = (org + pos) & vid_mask;
	set_VDC(CURSOR, offset >> 1);
}

//display a char, handle special chars \n, \r. \b
int putc(char c)
{
	u16 w, pos;
	if(c == '\n')
	{
		row++;
		if(row>=24)
		{
			row = 23;
			scroll();
		}
		
		move_cursor();
		return;
	}
	
	if(c == '\r')
	{
		column = 0;
		move_cursor();
		return;
	}
	
	if(c == '\b')
	{
		if(column > 0)
		{
			column--;
			move_cursor();
			put_word(0x0700, base, offset);
		}
		
		return;
	}
	
	//c if an ordinary char
	pos = 2*(row*80 + column);
	offset = (org + pos) & vid_mask;
	w = (color << 8) + c;
	put_word(w, base, offset);
	column++;
	
	if(column >= 80)
	{
		column = 0;
		row++;
		if(row>=24)
		{
			row = 23;
			scroll();
		}
	}
	
	move_cursor();
}

int set_VDC(u16 reg, u16 val) // set VDC register reg to val
{
	lock();
	out_byte(VDC_INDEX, reg);			//set the index register
	out_byte(VDC_DATA, (val>>8)&0xFF);	//output high byte
	out_byte(VDC_INDEX, reg + 1);		//next index register
	out_byte(VDC_DATA, val&0xFF);		//output low byte
	unlock();
}

//sets the time at the offset
void set_timeValue(int byteOff, char val)
{
	u16 pos, w;
	
	pos = 2*(24*80 + byteOff);
	offset = (org + pos) & vid_mask;
	w = (color << 8) + val;
	put_word(w, base, offset);
}

void remove_time()
{
	int borderOffset = 80;
	int i;
	
	for(i = 1; i < 9; i++)
	{
		set_timeValue(borderOffset - i, 0x0700);
	}
}

void set_time(u16 hours, u16 minutes, u16 seconds)
{
	char th, oh, tm, om, ts, os;
	int borderOffset = 80;
	
	th = ((hours % 100) / 10) + 48;
	oh = (hours % 10) + 48;
	
	tm = ((minutes % 100) / 10) + 48;
	om = (minutes % 10) + 48;
	
	ts = ((seconds % 100) / 10) + 48;
	os = (seconds % 10) + 48;	
	
	set_timeValue(borderOffset-1, os);
	set_timeValue(borderOffset-2, ts);
	
	set_timeValue(borderOffset-3, ':');

	set_timeValue(borderOffset-4, om);
	set_timeValue(borderOffset-5, tm);
	
	set_timeValue(borderOffset-6, ':');
	
	set_timeValue(borderOffset-7, oh);
	set_timeValue(borderOffset-8, th);
}
