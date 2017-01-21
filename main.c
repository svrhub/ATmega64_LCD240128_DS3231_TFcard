/*
 * ATmega64_LCD240128_DS3231_TFcard
 *
 * Created: 2016/2/22 21:47:15
 * Author : Stone
 */ 
#define  F_CPU 8000000UL
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
typedef unsigned char PROGMEM prog_uchar;

#include "LCD240128.h"
#include "IIC.h"
#include "DS3231.h"
#include "sdcard/ff.h"


FATFS FatFs;
FIL *fp;
FRESULT scan_files (char* path);
FRESULT get_file_size();
FRESULT read();
FRESULT write();




int main(void)
{
	DDRA=0b11111010;
	DDRC=0xff;
	PORTA=0x00;
	unsigned char temp1=0,temp2=0,i=0;
	Initialize_DS3231();
	//WriteTime_DS3231();
	lcd_init();
	lcd_clear(0x00);
	displayString_16x16(0,0,"1");	
	//FIL fil;       /* File object */
	char line[100]={0}; /* Line buffer */
	FRESULT fr;    /* FatFs return code */
	fr=f_mount(0,&FatFs);
	fp = (FIL *)malloc(sizeof (FIL));
	fr=f_open(fp,"/test1.txt", FA_READ);
	_delay_ms(2);		//等待文件打开
	if (fr)
	{
		displayString_16x16(0,0,"2");
	}
	_delay_ms(1000);
	displayString_16x16(0,0,"3");
	char text[255]={0};
	UINT br;
	f_read(fp, text, 255, &br);
	f_close(fp);
	text[30]=0;
	displayString_16x16(0,32,text);
	
	UINT bw;
	if (f_open(fp, "file.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {	// Create a file
		char *text = "Hello World! SDCard support up and running!\r\n";
		f_write(fp, text, strlen(text), &bw);	// Write data to the file
		f_close(fp);// Close the file
	}
	/*while (1)
	{	
		Time_DisplayString();
		displayString_16x16(0,16,DS3231_Time);
		_delay_ms(100);
	}	*/
}

