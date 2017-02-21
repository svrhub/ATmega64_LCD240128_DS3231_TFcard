/*
 * AD.h
 * ADת��
 * Created: 2015/11/8 12:25:04
 *  Author: stone
 */ 


#ifndef AD_H_
#define AD_H_
/********************************************* 
ADC��·������ѡ��Ĵ��� 
********************************************* 

 ADMUX (ADC Multiplexer Select Register) 
    bit7   bit6 
    REFS1  REFS0					�ο���ѹѡ�� 
    0       0						AREF���ڲ�Vref�ر� 
    0       1						AVCC��AREF��������˲�����  
    1       0						���� 
    1       1						2.56V��Ƭ�ڻ�׼��ѹԴ��AREF��������˲�����
	  
    bit5 
	ADLAR							ADC��������ѡ��  1=�����   0=�Ҷ���    
	 
    MUX3	MUX2	MUX1	MUX0	ѡ��ADͨ��
	0		0		0		0		ADC0
	0		0		0		1		ADC1
	.
	.
	.
	0		1		1		1		ADC7     

********************************************* 
ADC���ƺ�״̬�Ĵ��� 
*********************************************    

ADCSRA(ADC  Control and Status Register A) 
    bit7	ADEN	ADCʹ��=1 
    bit6	ADSC	����ADC��ʼת��=1 
    bit5	ADFR	����ת��ѡ�� 
    bit4	ADIF	ADC�жϱ�־ 
    bit3	ADIE	ADC�ж�ʹ�� 
    bit2	ADPS2	Ԥ��Ƶѡ��λ	000=2��Ƶ	001=2��Ƶ	111=128��Ƶ 
	bit1	ADPS1					010=4��Ƶ	011=8��Ƶ	100=16��Ƶ
	bit0	ADPS0
*/  
  
/********************************* 
ADC��ʼ�� 
example: adc_init(0); ��ʼ��ADC0
*********************************/  
void adc_init(unsigned char adcx)  
{
	PORTF	&=~(1<<adcx);
	DDRF	&=~(1<<adcx);    
	ADMUX	= (1<<REFS0)|(1<<REFS1)|adcx;			//ʹ���ڲ�2.56V�ο���ѹ
	ADCSRA	= (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1);
}  

unsigned int ADC_read(unsigned char adcx)
{	
	unsigned int ADC_result;
	adc_init(adcx);	
	while(!(ADCSRA&(1<<ADIF)));	
	ADC_result=ADCL;
	ADC_result=ADCH*256+ADC_result;
	ADCSRA &= ~(1<<ADIF);       //���־
	ADCSRA &= ~(1<<ADEN);       //�ر�ת��
	return ADC_result;
}


#endif /* AD_H_ */