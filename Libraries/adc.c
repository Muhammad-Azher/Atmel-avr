/*
 * adc.c
 *
 *  Created on:  2019
 *      Author: Azher
 */

#include "adc.h"

#define ADC_PORT 				PORTF		// All sensors connected on PORTF
#define TEM_SENSOR_PIN 			2			
#define LIGHT_SENSOR_PIN 		4
#define JOYSTICK_PIN			5
#define MICROPHONE_PIN_1		0
#define MICROPHONE_PIN_2		1
#define ADC_VREF_SRC  			1.6
#define ADC_PRESCALE            0x04
#define ADC_TEMP_MAX 			40
#define ADC_TEMP_MIN 			82
#define ADC_TEMP_RAW_MAX 		205
#define ADC_TEMP_RAW_MIN 		1000
#define ADC_TEMP_FACTOR 		10

void adc_init(void)
{
	DDR_REGISTER(ADC_PORT) &= ((0<< TEM_SENSOR_PIN) | (0<< LIGHT_SENSOR_PIN) | (0<<JOYSTICK_PIN) |(0<<MICROPHONE_PIN_1) | (0<<MICROPHONE_PIN_2));
	ADC_PORT |= ((0<< TEM_SENSOR_PIN) | (0<< LIGHT_SENSOR_PIN) | (0<<JOYSTICK_PIN) |(0<<MICROPHONE_PIN_1) | (0<<MICROPHONE_PIN_2));
	PRR0 &= (0<<PRADC);						// enabling adc
	ADMUX &= (0<<ADLAR);					// 
	ADMUX |= (1<<REFS1)|(1<<REFS0);
	ADCSRA &= (0<<ADATE);
	ADCSRA |= ADC_PRESCALE;
	ADCSRA |= (1<<ADEN);
}

uint16_t adc_read(uint8_t adc_channel)
{
	if(adc_channel > ADC_NUM) return ADC_INVALID_CHANNEL;
	else{
	adc_channel &= 0x1F;
	ADMUX &= 0xE0;
	ADMUX |= adc_channel;
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return(ADC);}
}

uint8_t adc_getJoystickDirection()
{
	return ((adc_read(ADC_JOYSTICK_CH)/201)-1);
}

int16_t adc_getTemperature()
{
	int16_t adc = adc_read(ADC_TEMP_CH);
	int16_t slope = (ADC_TEMP_MAX - ADC_TEMP_MIN)/(ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN);
	int16_t offset = ADC_TEMP_MAX - (ADC_TEMP_RAW_MAX * slope);
	return (adc * slope + offset) / ADC_TEMP_FACTOR;
}
