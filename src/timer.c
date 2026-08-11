#include "device_driver.h"

#define TIM2_TICK         	(20)
#define TIM2_FREQ 	  		(1000000/TIM2_TICK)
#define TIME2_PLS_OF_1ms  	(1000/TIM2_TICK)
#define TIM2_MAX	  		(0xffffu)

#define TIM4_TICK	  		(20)
#define TIM4_FREQ 	  		(1000000/TIM4_TICK)
#define TIME4_PLS_OF_1ms  	(1000/TIM4_TICK)
#define TIM4_MAX	  		(0xffffu)

void TIM2_Stopwatch_Start(void)
{
	Macro_Set_Bit(RCC->APB1ENR, 0);

	TIM2->CR1 = (1<<4)|(1<<3);
	TIM2->PSC = (unsigned int)(TIMXCLK/50000.0 + 0.5)-1;
	TIM2->ARR = TIM2_MAX;

	Macro_Set_Bit(TIM2->EGR,0);
	Macro_Set_Bit(TIM2->CR1, 0);
}

unsigned int TIM2_Stopwatch_Stop(void)
{
	unsigned int time;

	Macro_Clear_Bit(TIM2->CR1, 0);
	time = (TIM2_MAX - TIM2->CNT) * TIM2_TICK;
	return time;
}


void TIM2_Delay(int time)
{
	int i;
	unsigned int t = TIME2_PLS_OF_1ms * time;

	Macro_Set_Bit(RCC->APB1ENR, 3);

	TIM5->PSC = (unsigned int)(TIMXCLK/(double)TIM2_FREQ + 0.5)-1;
	TIM5->CR1 = (1<<4)|(1<<3);
	TIM5->ARR = 0xffff;
	Macro_Set_Bit(TIM5->EGR,0);

	for(i=0; i<(t/0xffffu); i++)
	{
		Macro_Set_Bit(TIM5->EGR,0);
		Macro_Clear_Bit(TIM5->SR, 0);
		Macro_Set_Bit(TIM5->CR1, 0);
		while(Macro_Check_Bit_Clear(TIM5->SR, 0));
	}

	TIM5->ARR = t % 0xffffu;
	Macro_Set_Bit(TIM5->EGR,0);
	Macro_Clear_Bit(TIM5->SR, 0);
	Macro_Set_Bit(TIM5->CR1, 0);
	while (Macro_Check_Bit_Clear(TIM5->SR, 0));

	Macro_Clear_Bit(TIM5->CR1, 0);
}

void TIM4_Repeat(int time)
{
	Macro_Set_Bit(RCC->APB1ENR, 2);

	TIM4->CR1 = (1<<4)|(0<<3);
	TIM4->PSC = (unsigned int)(TIMXCLK/(double)TIM4_FREQ + 0.5)-1;
	TIM4->ARR = TIME4_PLS_OF_1ms * time - 1;

	Macro_Set_Bit(TIM4->EGR,0);
	Macro_Clear_Bit(TIM4->SR, 0);
	Macro_Set_Bit(TIM4->CR1, 0);
}

int TIM4_Check_Timeout(void)
{
	if(Macro_Check_Bit_Set(TIM4->SR, 0))
	{
		Macro_Clear_Bit(TIM4->SR, 0);
		return 1;
	}
	else
	{
		return 0;
	}
}

void TIM4_Stop(void)
{
	Macro_Clear_Bit(TIM4->CR1, 0);
}

void TIM4_Change_Value(int time)
{
	TIM4->ARR = TIME4_PLS_OF_1ms * time;
}

void TIM4_Repeat_Interrupt_Enable(int en, int time)
{
	if(en)
	{
		// TIM4 Clock On
		Macro_Set_Bit(RCC->APB1ENR, 2);

		TIM4->CR1 = (1<<4)|(0<<3);
		TIM4->PSC = (unsigned int)(TIMXCLK/(double)TIM4_FREQ + 0.5)-1;
		TIM4->ARR = TIME4_PLS_OF_1ms * time;
		Macro_Set_Bit(TIM4->EGR,0);

		// TIM4 Pending Clear
		Macro_Clear_Bit(TIM4->SR, 0);

		// NVIC Pending Clear
		NVIC_ClearPendingIRQ(30);

		// TIM4 Interrupt Enable
		Macro_Set_Bit(TIM4->DIER, 0);

		// NVIC Interrupt Enable
		NVIC_EnableIRQ(30);

		// TIM4 Start
		Macro_Set_Bit(TIM4->CR1, 0);
	}
	else
	{
		NVIC_DisableIRQ(30);
		Macro_Clear_Bit(TIM4->CR1, 0);
		Macro_Clear_Bit(TIM4->DIER, 0);
	}
}

#define TIM3_FREQ 	  			(8000000)
#define TIM3_TICK	  			(1000000/TIM3_FREQ)
#define TIME3_PLS_OF_1ms  		(1000/TIM3_TICK)

void TIM3_Out_Init(void)
{
	Macro_Set_Bit(RCC->AHB1ENR, 1);
	Macro_Set_Bit(RCC->APB1ENR, 1);

	Macro_Write_Block(GPIOB->MODER, 0x3, 0x2, 0);
	Macro_Write_Block(GPIOB->AFR[0], 0xf, 0x2, 0);

	Macro_Write_Block(TIM3->CCMR2,0xff, 0x60, 0);
	TIM3->CCER = (0<<9)|(1<<8);
}

void TIM3_Out_Freq_Generation(unsigned short freq)
{
	TIM3->PSC = (unsigned int)(TIMXCLK/(double)TIM3_FREQ + 0.5)-1;
	TIM3->ARR = (double)TIM3_FREQ/freq-1;
	TIM3->CCR3 = TIM3->ARR/2;

	Macro_Set_Bit(TIM3->EGR,0);
	TIM3->CR1 = (1<<4)|(0<<3)|(0<<1)|(1<<0);
}

void TIM3_Out_Stop(void)
{
	Macro_Clear_Bit(TIM3->CR1, 0);
}

volatile int g_lockout_done = 0;

void TIM4_IRQHandler(void)
{
	Macro_Clear_Bit(TIM4->SR, 0);
	TIM4_Repeat_Interrupt_Enable(0, 0);
	g_lockout_done = 1;
}
