#include "device_driver.h"
#include "matrix.h"
#include <stdio.h>

#define PASSWORD    "1234"
#define MAX_FAIL    3
#define LOCK_TIME   10000

typedef enum
{
	ST_IDLE,
	ST_INPUT,
	ST_CHECK,
	ST_UNLOCK,
	ST_LOCKOUT
} STATE;

void Main(void)
{
	STATE state = ST_IDLE;
	char  input[5];
	int   idx = 0;
	int   fail_cnt = 0;
	char  key;
	char  msg[13];

	SCB->CPACR |= (0x3 << 10*2)|(0x3 << 11*2);

	Clock_Init();

	Uart2_Init(115200);
	setvbuf(stdout, NULL, _IONBF, 0);
	printf("\n===== Smart Door Lock System =====\n");

	I2C1_Init(100000);
	LCD_Init();
	Servo_Init();
	Keypad_Init();
	Matrix_Init();

	Servo_SetAngle(0);
	Matrix_SetBuffer(ICON_LOCK);

	printf("System Ready. Waiting for password...\n");

	while(1)
	{
		switch(state)
		{
		case ST_IDLE:
			idx = 0;
			LCD_Clear();
			LCD_Print(0, 0, "Enter Password:");
			Matrix_SetBuffer(ICON_LOCK);
			state = ST_INPUT;
			break;

		case ST_INPUT:
			key = Keypad_Scan();

			if(key)
			{
				if(key == '#')
				{
					input[idx] = '\0';
					printf("[#] Confirm pressed (%d digit(s) entered) -> checking...\n", idx);
					state = ST_CHECK;
				}
				else if(key == '*')
				{
					printf("[*] Cancel pressed -> input cleared\n");
					idx = 0;
					LCD_Print(1, 0, "                ");
				}
				else if(idx < 4)
				{
					input[idx++] = key;
					printf("Key input %d/4 received\n", idx);
					LCD_SetCursor(1, idx - 1);
					LCD_Data('*');
				}

				TIM2_Delay(200);
			}
			break;

		case ST_CHECK:
			if(idx == 4 && strcmp(input, PASSWORD) == 0)
			{
				printf(">>> ACCESS GRANTED\n");
				state = ST_UNLOCK;
			}
			else
			{
				fail_cnt++;
				printf(">>> ACCESS DENIED (fail %d/%d)\n", fail_cnt, MAX_FAIL);

				Matrix_SetBuffer(ICON_X);

				if(fail_cnt >= MAX_FAIL)
				{
					state = ST_LOCKOUT;
				}
				else
				{
					strcpy(msg, "Wrong! (X/3)");
					msg[8] = '0' + fail_cnt;

					LCD_Clear();
					LCD_Print(0, 0, msg);
					TIM2_Delay(1500);
					state = ST_IDLE;
				}
			}
			break;

		case ST_UNLOCK:
			LCD_Clear();
			LCD_Print(0, 0, "Access Granted!");
			Matrix_SetBuffer(ICON_CHECK);
			printf("Servo unlocking (90deg) for 3 sec...\n");
			Servo_SetAngle(90);
			TIM2_Delay(3000);
			Servo_SetAngle(0);
			printf("Servo locked (0deg). Ready.\n");
			fail_cnt = 0;
			state = ST_IDLE;
			break;

		case ST_LOCKOUT:
			LCD_Clear();
			LCD_Print(0, 0, "Locked. Wait...");
			Matrix_SetBuffer(ICON_X);
			printf("!!! %d failed attempts -> LOCKOUT for %d sec !!!\n",
				MAX_FAIL, LOCK_TIME / 1000);

			g_lockout_done = 0;
			TIM4_Repeat_Interrupt_Enable(1, LOCK_TIME);

			while(!g_lockout_done);

			printf("Lockout ended. System ready again.\n");
			fail_cnt = 0;
			state = ST_IDLE;
			break;
		}
	}
}
