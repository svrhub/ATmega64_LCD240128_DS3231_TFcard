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
#include <stdlib.h>
typedef unsigned char PROGMEM prog_uchar;
typedef char PROGMEM prog_char;

#include "xitoa/xitoa.h"
#include "PortInit.h"
#include "AD.h"
#include "LCD240128.h"
#include "IIC.h"
#include "DS3231.h"
#include "sdcard/ff.h"
unsigned char FLAG_Disk_Init=0;		/*SD����ʼ����ǣ�����Ѿ�ִ�й���ʼ����������������ó�ʼ����ֱ������    0:δ��ʼ��  1:�ѳ�ʼ��   */

FATFS FatFs;
FIL *fp;
FRESULT fr;
char *filename;
void Time_GUI();
FRESULT read_data();
unsigned char Create_File();
unsigned char Reopen_File();
void Display_Chart();


int main(void)
{	
	unsigned char Flag_Second=0,Flag_Minute=0,Flag_Hour=0,Flag_Write_OK=0;
	char text[28]={0};
	int Data_Of_Writen=0;
	
	
	Port_Init();			//�˿ڳ�ʼ��
	Initialize_DS3231();	//ʱ��оƬDS3231��ʼ��
	lcd_init();				//LCM240128Һ����ʼ��
	lcd_clear(0x00);		//Һ������
	
	drawLine(0,0,239,0,1);
	drawLine(0,17,239,17,1);
	drawLine(0,35,239,35,1);
	drawLine(128,0,128,34,1);
	drawLine(0,0,0,35,1);
	drawLine(239,0,239,35,1);
	
	fp = (FIL *)malloc(sizeof (FIL));
	filename=(char *)malloc(14);
	f_mount(0,&FatFs);		//�����ļ�ϵͳ������
	
	Create_File();			
	while (1)
	{	
		Time_DisplayString();			
		if (Flag_Second!=Time.second)
		{
			Flag_Second=Time.second;
			Time_GUI();					
		}
		if (!(Time.minute&0x0f) && Flag_Minute!=Time.minute)
		{	
			Flag_Minute=Time.minute;
			Data_Of_Writen=f_printf(fp,"%s,%s,%s %c", DS3231_Date,DS3231_Time,DS3231_Temperature,'\n');		//��ʽ������ַ�������ӻ��з�����д���ļ�
			if (Data_Of_Writen==-1)				//д��ʧ��
			{
				displayString_16x16(6,100,"111111");
				f_close(fp);
				do
				{
					Flag_Write_OK=Reopen_File();
					_delay_ms(100);
				} while (Flag_Write_OK);
				Data_Of_Writen=f_printf(fp,"%s,%s,%s %c", DS3231_Date,DS3231_Time,DS3231_Temperature,'\n');		//�ٴ�д��ղ�д��ʧ�ܵ�����
			}
			f_sync(fp);
			if (fp->fsize>409600)			//����ļ���С����400KB,�ر�ԭ�ļ������´����ļ�
			{
				f_close(fp);				//�ر��ļ�
				Create_File();				//�������ļ�
			}
			xsprintf(text, PSTR("Size:%06ldB"), fp->fsize);
			displayString_16x16(17,19,(unsigned char *)text);
		}
		if (Flag_Hour!=Time.hour)
		{
			Flag_Hour=Time.hour;
			Display_Chart();				
		}
	}
}


FRESULT read_data(){
	unsigned char text[255];
	UINT br;
	f_mount(0,&FatFs);
	fp = (FIL *)malloc(sizeof (FIL));
	if (f_open(fp, "file.txt", FA_READ|FA_WRITE) == FR_OK) {
		while (f_gets(text, sizeof text, &br))
		{displayString_16x16(16,96,text);}		
		f_close(fp);
	}
	f_mount(0, NULL);	
	return 0;
}

void Time_GUI(){
	//unsigned int Voltage=0;
	displayString_16x16(1,1,DS3231_Date);
	displayString_16x16(17,1,DS3231_Week);
	displayString_16x16(25,1,DS3231_Temperature);
	displayString_32x64(0,36,DS3231_Time);
	
	//Voltage=ADC_read(0);
	//xsprintf(text, PSTR("%d"), Voltage/2);
	//displayString_16x16(0,16,(unsigned char *)text);
}

unsigned char Create_File()
{
	ReadTime_DS3231();
	xsprintf(filename, PSTR("%02X%02X%02X.txt"), Time.year,Time.month,Time.date);		//����ļ�������ʽΪYYMMDD.TXT��	
	if (f_open(fp,(TCHAR *)filename, FA_CREATE_ALWAYS|FA_WRITE)) return 1;				//�����ļ�
	f_lseek(fp, f_size(fp));															//��д��λ��ָ��
	displayString_16x16(1,19,"File:");
	displayString_16x16(6,19,(unsigned char *)filename);
	return 0;
}

unsigned char Reopen_File()
{	
	FLAG_Disk_Init=0;
	f_mount(0,NULL);
	f_mount(0,&FatFs);
	if (f_open(fp,(TCHAR *)filename, FA_READ|FA_WRITE)) return 1;				//���´��ļ�
	f_lseek(fp, f_size(fp));
	return 0;
}

void Display_Chart()
{	
	static unsigned char Temper_Data[241]={0},Temper_Index=0;
	for (Temper_Index=0;Temper_Index<240;Temper_Index++)
	{	
		Lcd_drawPixel(Temper_Index,135-Temper_Data[Temper_Index],0);
		Temper_Data[Temper_Index]=Temper_Data[Temper_Index+1];		
		Temper_Data[240]=Temperature_Integer;
		Lcd_drawPixel(Temper_Index,135-Temper_Data[Temper_Index],1);
	}	
}