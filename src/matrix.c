#include "device_driver.h"
#include "matrix.h"

static unsigned char frame[8];
static volatile int cur_row = 0;

static void Row_All_Off(void)
{
	int r;
	for(r = 0; r < 6; r++)
		Macro_Clear_Bit(GPIOB->ODR, r);
	Macro_Clear_Bit(GPIOC->ODR, 4);
	Macro_Clear_Bit(GPIOC->ODR, 5);
}

static void Row_On(int row)
{
	if(row < 6)
		Macro_Set_Bit(GPIOB->ODR, row);
	else if(row == 6)
		Macro_Set_Bit(GPIOC->ODR, 4);
	else
		Macro_Set_Bit(GPIOC->ODR, 5);
}

static void Matrix_Refresh(void)
{
	unsigned int col_bits;

	Row_All_Off();

	col_bits = (~frame[cur_row]) & 0xFF;
	Macro_Write_Block(GPIOB->ODR, 0xff, col_bits, 8);

	Row_On(cur_row);

	cur_row = (cur_row + 1) % 8;
}

static void TIM3_AutoRefresh_Init(void)
{
	Macro_Set_Bit(RCC->APB1ENR, 1);

	TIM3->PSC = (unsigned int)(TIMXCLK / 1000000.0 + 0.5) - 1;
	TIM3->ARR = 1000 - 1;
	Macro_Set_Bit(TIM3->EGR, 0);
	Macro_Clear_Bit(TIM3->SR, 0);

	NVIC_ClearPendingIRQ(29);
	Macro_Set_Bit(TIM3->DIER, 0);
	NVIC_EnableIRQ(29);

	Macro_Set_Bit(TIM3->CR1, 0);
}

void TIM3_IRQHandler(void)
{
	Macro_Clear_Bit(TIM3->SR, 0);
	Matrix_Refresh();
}

void Matrix_Init(void)
{
	Macro_Set_Bit(RCC->AHB1ENR, 1);
	Macro_Set_Bit(RCC->AHB1ENR, 2);

	// PB0~PB5 (Row1~6)
	Macro_Write_Block(GPIOB->MODER, 0xfff, 0x555, 0);
	Macro_Write_Block(GPIOB->OTYPER, 0x3f, 0x00, 0);

	// PB8~PB15 (Col1~8)
	Macro_Write_Block(GPIOB->MODER, 0xffff, 0x5555, 16);
	Macro_Write_Block(GPIOB->OTYPER, 0xff, 0x00, 8);

	// PC4, PC5 (Row7,8)
	Macro_Write_Block(GPIOC->MODER, 0xf, 0x5, 8);
	Macro_Write_Block(GPIOC->OTYPER, 0x3, 0x0, 4);

	GPIOB->ODR = 0x0000;
	Row_All_Off();

	{
		int i;
		for(i = 0; i < 8; i++)
			frame[i] = 0x00;
	}

	TIM3_AutoRefresh_Init();
}

void Matrix_SetBuffer(const unsigned char rows[8])
{
	int i;
	for(i = 0; i < 8; i++)
		frame[i] = rows[i];
}

// matrix icon

const unsigned char ICON_LOCK[8] = {
	0b00111100,
	0b01000010,
	0b01000010,
	0b11111111,
	0b11100111,
	0b11100111,
	0b11100111,
	0b11111111,
};

const unsigned char ICON_CHECK[8] = {
	0b00111100,
	0b01111110,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b01111110,
	0b00111100,
};

const unsigned char ICON_X[8] = {
	0b10000001,
	0b01000010,
	0b00100100,
	0b00011000,
	0b00011000,
	0b00100100,
	0b01000010,
	0b10000001,
};

const unsigned char ICON_BLANK[8] = {0,0,0,0,0,0,0,0};
