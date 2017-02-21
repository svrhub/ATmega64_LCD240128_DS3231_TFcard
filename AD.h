/*
 * AD.h
 * AD转换
 * Created: 2015/11/8 12:25:04
 *  Author: stone
 */ 


#ifndef AD_H_
#define AD_H_
/********************************************* 
ADC多路复用器选择寄存器 
********************************************* 

 ADMUX (ADC Multiplexer Select Register) 
    bit7   bit6 
    REFS1  REFS0					参考电压选择 
    0       0						AREF，内部Vref关闭 
    0       1						AVCC，AREF引脚外加滤波电容  
    1       0						保留 
    1       1						2.56V的片内基准电压源，AREF引脚外加滤波电容
	  
    bit5 
	ADLAR							ADC结果左对齐选择  1=左对齐   0=右对齐    
	 
    MUX3	MUX2	MUX1	MUX0	选择AD通道
	0		0		0		0		ADC0
	0		0		0		1		ADC1
	.
	.
	.
	0		1		1		1		ADC7     

********************************************* 
ADC控制和状态寄存器 
*********************************************    

ADCSRA(ADC  Control and Status Register A) 
    bit7	ADEN	ADC使能=1 
    bit6	ADSC	启动ADC开始转换=1 
    bit5	ADFR	连续转换选择 
    bit4	ADIF	ADC中断标志 
    bit3	ADIE	ADC中断使能 
    bit2	ADPS2	预分频选择位	000=2分频	001=2分频	111=128分频 
	bit1	ADPS1					010=4分频	011=8分频	100=16分频
	bit0	ADPS0
*/  
  
/********************************* 
ADC初始化 
example: adc_init(0); 初始化ADC0
*********************************/  
void adc_init(unsigned char adcx)  
{
	PORTF	&=~(1<<adcx);
	DDRF	&=~(1<<adcx);    
	ADMUX	= (1<<REFS0)|(1<<REFS1)|adcx;			//使用内部2.56V参考电压
	ADCSRA	= (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1);
}  

unsigned int ADC_read(unsigned char adcx)
{	
	unsigned int ADC_result;
	adc_init(adcx);	
	while(!(ADCSRA&(1<<ADIF)));	
	ADC_result=ADCL;
	ADC_result=ADCH*256+ADC_result;
	ADCSRA &= ~(1<<ADIF);       //清标志
	ADCSRA &= ~(1<<ADEN);       //关闭转换
	return ADC_result;
}


#endif /* AD_H_ */