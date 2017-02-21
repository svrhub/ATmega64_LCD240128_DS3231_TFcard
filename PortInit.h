/*
 * SysInit.h
 *
 * Created: 2017/1/24 0:04:28
 *  Author: Stone
 */ 


#ifndef SYSINIT_H_
#define SYSINIT_H_

void Port_Init()
{
	DDRA=0b11111010;
	DDRC=0xff;
	PORTA=0x00;
	DDRD|=(1<<PORTD1);
}




#endif /* SYSINIT_H_ */