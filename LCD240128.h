/*
 * LCD240128.h
 *
 * Created: 2016/2/22 21:48:46
 *  Author: Stone
 */ 


#ifndef LCD240128_H_
#define LCD240128_H_
#include "font.h"
#define NOP()	asm("nop")

#define DATA_PORT		PORTC
#define READ_DATA_PORT	PINC
#define DDR_IN			DDRC=0x00
#define DDR_OUT			DDRC=0xff
#define CMD_PORT		PORTA
#define LCD_RW          PORTA7
#define LCD_RD          PORTA6
#define LCD_CE          PORTA5
#define LCD_CD          PORTA4
#define LCD_RES         PORTA3


#define LCD_RW_H	CMD_PORT|=(1<<LCD_RW)
#define LCD_RW_L	CMD_PORT&=~(1<<LCD_RW)
#define LCD_RD_H	CMD_PORT|=(1<<LCD_RD)
#define LCD_RD_L	CMD_PORT&=~(1<<LCD_RD)
#define LCD_CE_H	CMD_PORT|=(1<<LCD_CE)
#define LCD_CE_L	CMD_PORT&=~(1<<LCD_CE)
#define LCD_CD_H	CMD_PORT|=(1<<LCD_CD)
#define LCD_CD_L	CMD_PORT&=~(1<<LCD_CD)
#define LCD_RES_H	CMD_PORT|=(1<<LCD_RES)
#define LCD_RES_L	CMD_PORT&=~(1<<LCD_RES)


void LCD_Busy()
{
	LCD_CD_H;NOP();NOP();NOP();					// CD高，读状态字节
	LCD_CE_L;NOP();NOP();NOP();					// CE低，开片选信号
	LCD_RD_L;NOP();NOP();NOP();					// RD低，打开读信号
	DDR_IN;
	while((READ_DATA_PORT & (1<<LCD_RW)) ==0); NOP();NOP();NOP();      // 读取状态字节
	DDR_OUT;NOP();NOP();NOP();
	LCD_RD_H;NOP();NOP();NOP();   	 				// 关闭读信号
	LCD_CE_H;						 			    // CE高，关闭片选
}


void LCD_SendCom(unsigned char lcd_com)
{
	LCD_Busy();
	LCD_RD_H;NOP();NOP();NOP();
	LCD_CD_H;NOP();NOP();NOP();
	LCD_RW_L;NOP();NOP();NOP();
	LCD_CE_H;NOP();NOP();NOP();
	DATA_PORT=lcd_com;NOP();NOP();NOP();
	LCD_CE_L;NOP();NOP();NOP();
	LCD_CE_H;NOP();NOP();NOP();
	LCD_RW_H;NOP();NOP();NOP();
	LCD_CD_L;
}

void LCD_SendData(unsigned char lcd_data)
{
	LCD_Busy();
	LCD_RD_H;NOP();NOP();NOP();
	LCD_CD_L;NOP();NOP();NOP();
	LCD_RW_L;NOP();NOP();NOP();
	LCD_CE_H;NOP();NOP();NOP();
	DATA_PORT=lcd_data;NOP();NOP();NOP();
	LCD_CE_L;NOP();NOP();NOP();
	LCD_CE_H;NOP();NOP();NOP();
	LCD_RW_H;NOP();NOP();NOP();
	LCD_CD_H;
}

unsigned char LCD_ReadData()
{
	unsigned char lcd_data;
	LCD_Busy();
	LCD_CD_L;NOP();NOP();NOP();				
	LCD_RD_L;NOP();NOP();NOP();
	LCD_RW_H;NOP();NOP();NOP();
	LCD_CE_L;NOP();NOP();NOP();	
	DDR_IN;
	lcd_data=READ_DATA_PORT;NOP();NOP();NOP();
	DDR_OUT;NOP();NOP();NOP();
	LCD_RD_H;NOP();NOP();NOP();   	 				
	LCD_CE_H;
	LCD_CD_H;						 			    
	return lcd_data;
}

void LCD_SendDoubleByteCom(unsigned char lcd_com,unsigned char lcd_data1,unsigned char lcd_data2)
{
	LCD_SendData(lcd_data1);
	LCD_SendData(lcd_data2);
	LCD_SendCom(lcd_com);
}

void lcd_init(void)
{
	LCD_RES_H;
	LCD_RES_L;
	_delay_ms(10);
	LCD_RES_H;
	LCD_SendCom(0x80);          //设置显示模式为逻辑“或”模式
	LCD_SendCom(0x98);          //开显示
	LCD_SendDoubleByteCom(0x40,0,0);     //设置文本显示区首地址
	LCD_SendDoubleByteCom(0x41,30,0);    //设置文本显示区宽度
	LCD_SendDoubleByteCom(0x42,0,0);     //设置图形显示区首地址
	LCD_SendDoubleByteCom(0x43,30,0);    //设置图形显示区宽度
}

void lcd_clear(unsigned char lcd_data)
{
	unsigned char i,j;
	LCD_SendCom(0x98);                    /* 打开图形显示，关闭文本显示   */
	LCD_SendDoubleByteCom(0x24,0,0);      /* 设置地址指针 */
	LCD_SendCom(0xb0);                    /* 设置自动写数据 */
	for(i=0;i<128;i++)
	{
		for(j=0;j<30;j++)
		{
			LCD_SendData(lcd_data);/* Write display data */
		}
	}
	LCD_SendCom(0xb2);
}

void LCD_DisplayBMP()
{	
	LCD_SendCom(0x98); 
	LCD_SendDoubleByteCom(0x24,0,0);
	LCD_SendCom(0xb0);
	unsigned char i,j=0;
	for (i=0;i<128;i++)
	{	
		for (j=0;j<30;j++)
		{
			LCD_SendData(pgm_read_byte(&codeBMP240_128[i*30+j]));
		}
	}
}

void LCD_Display_8x16(unsigned char x,unsigned char y,unsigned char data)
{	
	LCD_SendCom(0x98);
	unsigned char i=0;
	unsigned int ADDR=0;
	for (i=0;i<16;i++)
	{	
		ADDR=(y+i)*30+x;
		LCD_SendDoubleByteCom(0x24,(unsigned char)ADDR,(unsigned char)(ADDR>>8));	/* 设置地址指针 */	
		LCD_SendData(pgm_read_byte(&codeGB8_16[(data-32)*16+i]));
		LCD_SendCom(0xC4);			//单次写入
	}
}

/*
 *	16X16单个汉字显示程序
 *	x	：横坐标（0-30），为方便显示汉字，此处设定为横向8列为1组，全屏幕16组
 *	y	：纵坐标（0-128），
 *	c[2]：汉字编码
 *	汉字的取样由codeGB16_16[]数组取，先遍历汉字编码，再取编码所在位置的32字节点阵编码按顺序写入屏幕
 */
void LCD_Display_16x16hz(unsigned char x,unsigned char y,unsigned char c[2])
{
	LCD_SendCom(0x98);
	unsigned char i,k=0;
	unsigned int ADDR=0;
	for (k=0; k<60; k++)
	{		
		if ((pgm_read_byte(&codeGB16_16[k].Index[0])==c[0])&&(pgm_read_byte(&codeGB16_16[k].Index[1])==c[1]))
		{
			for (i=0;i<16;i++)
			{
				ADDR=(y+i)*30+x;
				LCD_SendDoubleByteCom(0x24,(unsigned char)ADDR,(unsigned char)(ADDR>>8));	/* 设置地址指针 */
				LCD_SendData(pgm_read_byte(&codeGB16_16[k].Msk[i*2]));
				LCD_SendCom(0xC4);			//单次写入
				ADDR=(y+i)*30+x+1;
				LCD_SendDoubleByteCom(0x24,(unsigned char)ADDR,(unsigned char)(ADDR>>8));	/* 设置地址指针 */
				LCD_SendData(pgm_read_byte(&codeGB16_16[k].Msk[i*2+1]));
				LCD_SendCom(0xC4);			//单次写入
			}			
		}
	}
}


void displayString_16x16(unsigned char x,unsigned char y,unsigned char *s)
{
	while (*s)
	{	
		if(*s>127)
		{	
			if (x>0 && (x+1)%30==0){x++;}	//避免中文显示出现左半在上一行，右半在下一行的情况
			LCD_Display_16x16hz(x,y,s);
			s=s+2;
			x=x+2;
		}
		else
		{
			LCD_Display_8x16(x,y,*s);
			s++;
			x++;
		}
	}
}

void LCD_Display_32x64(unsigned char x,unsigned char y,unsigned char width,unsigned char data)
{
	LCD_SendCom(0x98);
	unsigned char i,j=0;
	unsigned int ADDR=0;
	for (i=0;i<64;i++)
	{	
		for (j=0;j<width;j++)
		{
			ADDR=(y+i)*30+x+j;
			LCD_SendDoubleByteCom(0x24,(unsigned char)ADDR,(unsigned char)(ADDR>>8));	/* 设置地址指针 */
			LCD_SendData(pgm_read_byte(&codeGB32_64[(data-48)*256+i*4+j]));
			LCD_SendCom(0xC4);			//单次写入
		}		
	}
}

void displayString_32x64(unsigned char x,unsigned char y,unsigned char *s)
{	
	unsigned char width=0;
	while (*s)
	{	
		width=(*s==':')?3:4;
		LCD_Display_32x64(x,y,width,*s);		
		s++;
		x=x+width;
	}
}
/*******************************************************/
/******   函数名称: lcd_dis_point()               ******/
/******   功    能: 指定位置画点或清点            ******/
/******   参    数: x(0~239),y(0~127)             ******/
/******             pointbit=0:清点pointbit=1:画点******/
/******   返回值  : 无                            ******/
/*******************************************************/
void Lcd_drawPixel(unsigned char x,unsigned char y, unsigned char pointbit)
{
	int addr;
	if( (x>240) || (y>128) )return;
	LCD_SendCom(0x98);
	addr =0x00+x/8+y*30;
	LCD_SendDoubleByteCom(0x24,addr%256,addr/256);
	if(pointbit)
	LCD_SendCom(0xf8 | (7-x%8));  // 0xf8为画点命令
	else
	LCD_SendCom(0xf0 | (7-x%8));  // 0xf0为清点命令
}

/************************************************************************
	任意位置画线 
	x1	:起点横坐标 0-127	y1	:起点纵坐标 0-127
	x2	:终点横坐标 0-127	y2	:终点纵坐标 0-127
	attr:0	消线
		:1	画线      
	使用的是Bresenham算法                                        
************************************************************************/
void drawLine(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char attr)
{
	int dx,dy,inc_x,inc_y,ds,i,xerr,yerr;
	
	
	dx=x2-x1;
	dy=y2-y1;
	
	if (dx==0)		//	此时为竖直线
	{	
		inc_x=0;
	}		
	if(!(dx>>15)){		//	设置步进方向
		inc_x=1;
	}	
	else
	{
		inc_x=-1;
		dx=-dx;
	}
	if (dy==0){		//	此时为水平线
		inc_y=0;
	}		
	if (!(dy>>15))
	{
		inc_y=1;
	}
	else
	{
		inc_y=-1;
		dy=-dy;
	}
		
	if (dx>dy){ds=dx;}
	else{ds=dy;}	//选取基本增量坐标轴
	
	xerr=dx;yerr=dy;
	for(i = 0; i <= ds; i++)				//画线输出
	{
		Lcd_drawPixel(x1, y1, attr);			//画点
		xerr += dx;
		yerr += dy;
		if(xerr > ds)
		{
			xerr -= ds;
			x1   += inc_x;			
		}
		if(yerr > ds)
		{
			yerr -= ds;
			y1   += inc_y;
		}
	}
}
/************************************************************************
	任意位置画圆 
	x	:圆心横坐标 0-127
	y	:圆心纵坐标 0-127
	r	:圆半径
	attr:0	消圆
		:1	画圆      
	使用的是Bresenham算法                                        
************************************************************************/
void drawCircle(unsigned char x, unsigned char y, unsigned char r, unsigned char attr)
{
	unsigned char dx, dy = r;
	for(dx = 0; dx <= r; dx++)
	{
		while((r * r + 1 - dx * dx) < (dy * dy)) dy--;
		Lcd_drawPixel(x + dx, y - dy, attr);
		Lcd_drawPixel(x - dx, y - dy, attr);
		Lcd_drawPixel(x - dx, y + dy, attr);
		Lcd_drawPixel(x + dx, y + dy, attr);
		Lcd_drawPixel(x + dy, y - dx, attr);
		Lcd_drawPixel(x - dy, y - dx, attr);
		Lcd_drawPixel(x - dy, y + dx, attr);
		Lcd_drawPixel(x + dy, y + dx, attr);
	}
}
#endif /* LCD240128_H_ */