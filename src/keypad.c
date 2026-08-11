#include "device_driver.h"

static const unsigned char row_pin[4] = {2, 3, 10, 11};
static const unsigned char col_pin[4] = {6, 7, 8, 9};

static const char keymap[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};

void Keypad_Init(void)
{
	int i;

	Macro_Set_Bit(RCC->AHB1ENR, 2);

	for(i = 0; i < 4; i++)
	{
		int p = row_pin[i];
		Macro_Write_Block(GPIOC->MODER, 0x3, 0x1, p*2);
		Macro_Clear_Bit(GPIOC->OTYPER, p);
		Macro_Write_Block(GPIOC->OSPEEDR, 0x3, 0x0, p*2);
		Macro_Set_Bit(GPIOC->ODR, p);
	}

	for(i = 0; i < 4; i++)
	{
		int p = col_pin[i];
		Macro_Write_Block(GPIOC->MODER, 0x3, 0x0, p*2);
		Macro_Write_Block(GPIOC->PUPDR, 0x3, 0x1, p*2);
	}
}

char Keypad_Scan(void)
{
	int row, col;

	for(row = 0; row < 4; row++)
	{
		int i;
		unsigned int col_val = 0;

		for(i = 0; i < 4; i++)
			Macro_Set_Bit(GPIOC->ODR, row_pin[i]);
		Macro_Clear_Bit(GPIOC->ODR, row_pin[row]);

		TIM2_Delay(1);

		for(i = 0; i < 4; i++)
			if(Macro_Check_Bit_Clear(GPIOC->IDR, col_pin[i]))
				col_val |= (1 << i);

		if(col_val)
		{
			for(col = 0; col < 4; col++)
			{
				if(col_val & (1 << col))
				{
					for(i = 0; i < 4; i++)
						Macro_Set_Bit(GPIOC->ODR, row_pin[i]);
					return keymap[row][col];
				}
			}
		}
	}

	for(row = 0; row < 4; row++)
		Macro_Set_Bit(GPIOC->ODR, row_pin[row]);

	return 0;
}


void Keypad_Debug_ScanAll(unsigned int col_vals[4])
{
	int row, i;

	for(row = 0; row < 4; row++)
	{
		unsigned int v = 0;

		for(i = 0; i < 4; i++)
			Macro_Set_Bit(GPIOC->ODR, row_pin[i]);
		Macro_Clear_Bit(GPIOC->ODR, row_pin[row]);

		TIM2_Delay(1);

		for(i = 0; i < 4; i++)
			if(Macro_Check_Bit_Clear(GPIOC->IDR, col_pin[i]))
				v |= (1 << i);

		col_vals[row] = v;
	}

	for(row = 0; row < 4; row++)
		Macro_Set_Bit(GPIOC->ODR, row_pin[row]);
}
