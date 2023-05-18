#include <reg51.h>
#include <stdio.h>
#define lcd_data P1
sbit rs=P3^5;
sbit rw=P3^6;
sbit en=P3^7;
sbit buz=P3^0;
sbit setc=P3^1;
sbit seta=P3^2;


unsigned int datime[3];
unsigned int alarm[3];
unsigned int sdigit[2];
unsigned int mdigit[2];
unsigned int hdigit[2];
unsigned long seconds = 0;
unsigned long alseconds =0;

// Keypad
sbit r1 = P0^0;
sbit r2 = P0^1;
sbit r3 = P0^2;
sbit r4 = P0^3;
sbit c1 = P0^4;
sbit c2 = P0^5;
sbit c3 = P0^6;

unsigned char keypad();
unsigned char key[4][3]={{'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};

unsigned char keypad()
{
char row,col;
unsigned char a;

r1=r2=r3=r4=0;
c1=c2=c3=1;
while(c1&&c2&&c3);
	while(c1==0||c2==0||c3==0)
	{
	r1=0;r2=r3=r4=1;if(c1==0||c2==0||c3==0){row=0;break;}
	r2=0;r1=r3=r4=1;if(c1==0||c2==0||c3==0){row=1;break;}
	r3=0;r1=r2=r4=1;if(c1==0||c2==0||c3==0){row=2;break;}
	r4=0;r2=r3=r1=1;if(c1==0||c2==0||c3==0){row=3;break;}
	}
	if(c1==0&&c2!=0&&c3!=0)col=0;
	else if(c2==0&&c1!=0&&c3!=0)col=1;
	else if(c3==0&&c2!=0&&c1!=0)col=2;
	while(c1==0||c2==0||c3==0);

	a=key[row][col];
	return a;

}

void del(unsigned int delay)
{
	int i,j;
	for(i=0; i<delay; i++)
	for(j=0;j<1275;j++);
}

void delay(unsigned int delay)
{
for(delay=0;delay<1275;delay++);
}

void cmd(unsigned char a)
{
lcd_data=a;
rs=0;
rw=0;
en=1;
delay(10);
en=0;
}

void lcd_init()
{
cmd(0x38); // 2 lines and 5x7 matrix
cmd(0x0e); // display on, cursor blinking
cmd(0x01); // clear display
cmd(0x06); // increment cursor (shift cursor to right)
cmd(0x0c); // display on, cursor off
cmd(0x80); // force cursor to beginning of first line
}


void dat(unsigned char b)
{
lcd_data=b;
rs=1;
rw=0;
en=1;
delay(10);
en=0;
}

void ho_min_sec(unsigned long sec) {
	datime[0] = sec/3600;
	sec = sec%3600;
	datime[1] = sec/60;
	sec = sec%60;
	datime[2] = sec; 
}
void sec_to_digits(unsigned long s) {
	sdigit[1] = s/10; // ten
	sdigit[0] = s%10; // unit
}

void min_to_digits(unsigned int m) {
	mdigit[1] = m/10;
	mdigit[0] = m%10;
}

void hour_to_digits(unsigned int h) {
	hdigit[1] = h/10;
	hdigit[0] = h%10;
}
void tochar(unsigned int value)
{
char tendigit,unitdigit;
tendigit=value/10;
if((tendigit+48)>='1')
dat(tendigit+48);
unitdigit=value%10;
dat(unitdigit+48);
}


void show(unsigned char *s)
{
	while(*s)
	{
	dat(*s++);
	}
}

void set_time(unsigned int h, unsigned int m, unsigned int s) {
	seconds = h*3600 + m*60 + s;
}
void set_altime(unsigned int h, unsigned int m) {
	alseconds = h*3600 + m*60;
}

void cl_set()
{
	unsigned char t1,t2,h,s,m;
			cmd(0x01);
			cmd(0x80);
			show("    SET TIME    ");
	L1:		cmd(0xc0);
			show("HH:");
			t1=keypad();
			dat(t1);
			t1=(t1-0x30)*10;
			t2=keypad();
			dat(t2);
			t2=t2-0x30;
			h=t1+t2;
				if(h>23)
				{
				cmd(0xc3);
				show("  ");
				goto L1;
				}
			dat(' ');
			del(100);
	L2:		cmd(0xc6);
			show("MM:");
			t1=keypad();
			dat(t1);
			t1=(t1-0x30)*10;
			t2=keypad();
			dat(t2);
			t2=t2-0x30;
			m=t1+t2;
			if(m>59)
				{
				cmd(0xc9);
				show("  ");
				goto L2;
				}
			del(100);
	L3:		cmd(0xcb);
			show("SS:");
			t1=keypad();
			dat(t1);
			t1=(t1-0x30)*10;
			t2=keypad();
			dat(t2);
			t2=t2-0x30;
			s=t1+t2;
			if(s>59)
				{
				cmd(0xce);
				show("  ");
				goto L3;
				}
			set_time(h,m,s);
			del(100);
}
void al_set()
{
	unsigned char t1,t2,h,m;
			cmd(0x01);
			cmd(0x80);
			show("    SET ALARM    ");
	L1:		cmd(0xc0);
			show("HH:");
			t1=keypad();
			dat(t1);
			t1=(t1-0x30)*10;
			t2=keypad();
			dat(t2);
			t2=t2-0x30;
			h=t1+t2;
				if(h>23)
				{
				cmd(0xc3);
				show("  ");
				goto L1;
				}
			dat(' ');
			del(100);
	L2:		cmd(0xc6);
			show("MM:");
			t1=keypad();
			dat(t1);
			t1=(t1-0x30)*10;
			t2=keypad();
			dat(t2);
			t2=t2-0x30;
			m=t1+t2;
			if(m>59)
				{
					cmd(0xc9);
					show("  ");
					goto L2;
				}
			set_altime(h,m);
			del(100);
			cmd(0x01);
}
void main(){
	char word[]="Welcome";
	int i=0;
	buz=1;
	lcd_init();
	for(i=0; i<7; i++) {
		dat(word[i]);
		delay(100);
	}
	del(150);
	// clear display
	cmd(0x01);
	cl_set();
	while(1) {
		cmd(0x01); // clear display
		if(setc == 0) cl_set();
		if(seta == 0) al_set();
		if(alseconds == seconds) buz=0;
		if(buz == 0) 
		{
			if(seconds - alseconds < 5) {
				buz=0;
			}
			else {
				buz=1;
			}
		}
		ho_min_sec(seconds);
		hour_to_digits(datime[0]);
		min_to_digits(datime[1]);
		sec_to_digits(datime[2]);
		tochar(hdigit[1]);
		tochar(hdigit[0]);
		dat(58);
		tochar(mdigit[1]);
		tochar(mdigit[0]);
		dat(58);
		tochar(sdigit[1]);
		tochar(sdigit[0]);
		del(100);
		seconds++;
	}
}