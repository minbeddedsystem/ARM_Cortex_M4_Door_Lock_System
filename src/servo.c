#include "device_driver.h"

#define SERVO_TICK_FREQ   1000000
#define SERVO_PERIOD_US   20000

void Servo_Init(void)
{
	Macro_Set_Bit(RCC->AHB1ENR, 0);
	Macro_Set_Bit(RCC->APB1ENR, 0);

	Macro_Write_Block(GPIOA->MODER, 0x3, 0x2, 0); 
	Macro_Write_Block(GPIOA->AFR[0], 0xf, 0x1, 0);

	TIM2->PSC = (unsigned int)(TIMXCLK/(double)SERVO_TICK_FREQ + 0.5)-1;
	TIM2->ARR = SERVO_PERIOD_US - 1;

	Macro_Write_Block(TIM2->CCMR1, 0xff, 0x60, 0);
	TIM2->CCER = (0<<1)|(1<<0);

	TIM2->CCR1 = 500;

	Macro_Set_Bit(TIM2->EGR, 0);
	TIM2->CR1 = (1<<4)|(0<<3)|(0<<1)|(1<<0);
}

void Servo_SetAngle(int angle)
{
	int pulse;

	if(angle < 0)   angle = 0;
	if(angle > 180) angle = 180;

	pulse = 500 + (angle * 2000 / 180);
	TIM2->CCR1 = pulse;
}
