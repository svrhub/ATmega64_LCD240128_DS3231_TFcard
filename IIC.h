/*
 * IIC.h
 *
 * Created: 2016/4/3 18:14:24
 *  Author: Stone
 */ 


#ifndef IIC_H_
#define IIC_H_

#define SCL_H		PORTA|=(1<<PORTA1)
#define SCL_L		PORTA&=~(1<<PORTA1)
#define SDA_H		PORTA|=(1<<PORTA0)
#define SDA_L		PORTA&=~(1<<PORTA0)
#define SDA_READ	(PINA&(1<<PORTA0))
#define SDA_IN		DDRA&=~(1<<PORTA0)
#define SDA_OUT		DDRA|=(1<<PORTA0)
#define I2C_DELAY	_delay_us(1)

#define NOP()	asm("nop")

void I2C_Start(void)
{
	SDA_OUT;
	SDA_H;
	SCL_H;
	I2C_DELAY;
	SDA_L;
	I2C_DELAY;
}

void I2C_Stop(void)
{
	SCL_L;
	I2C_DELAY;
	SDA_L;
	I2C_DELAY;
	SCL_H;
	I2C_DELAY;
	SDA_H;
	I2C_DELAY;
}

void I2C_Ack(void)
{
	SCL_L;
	I2C_DELAY;
	SDA_L;
	I2C_DELAY;
	SCL_H;
	I2C_DELAY;
	SCL_L;
	I2C_DELAY;
	SDA_H;
}

void I2C_NoAck(void)
{
	SCL_L;
	I2C_DELAY;
	SDA_H;
	I2C_DELAY;
	SCL_H;
	I2C_DELAY;
	SCL_L;
	I2C_DELAY;
	SDA_H;
}

//数据从高位到低位
void I2C_SendByte(unsigned char SendByte)
{
	unsigned char i=0;
	SDA_OUT;
	SCL_L;
	for (i=0;i<8;i++)
	{
		if(SendByte & 0x80)
		{
			SDA_H;
		}
		else
		{
			SDA_L;
		}
		SCL_H;
		I2C_DELAY;
		SendByte <<= 1;
		SCL_L;
	}
	SDA_H;
	SCL_H;
	I2C_DELAY;
	SCL_L;
	I2C_DELAY;
}

//数据从高位到低位
unsigned char I2C_ReceiveByte(void)
{
	unsigned char i=0;
	unsigned char ReceiveByte=0;
	SDA_IN;
	SDA_H;
	for (i=0;i<8;i++)
	{
		ReceiveByte <<= 1;
		SCL_H;NOP();NOP();NOP();NOP();NOP();NOP();NOP();
		if (SDA_READ)
		{
			ReceiveByte	|=1;
		}
		SCL_L;
	}	
	SDA_OUT;
	SDA_H;
	return ReceiveByte;
}

#endif /* IIC_H_ */