#include "stm32f4xx.h"
#include "option.h"
#include "macro.h"
#include "malloc.h"
#include <string.h>

/* ===================== SC16IS752 Register Address ===================== */

#define SC16IS752_IODIR     0x0A
#define SC16IS752_IOSTATE   0x0B

/* ============================ Uart.c ============================ */

extern void Uart2_Init(int baud);
extern void Uart2_Send_Byte(char data);

extern void Uart1_Init(int baud);
extern void Uart1_Send_Byte(char data);
extern void Uart1_Send_String(char *pt);
extern void Uart1_Printf(char *fmt,...);
extern char Uart1_Get_Char(void);
extern char Uart1_Get_Pressed(void);

/* =========================== SysTick.c =========================== */

extern void SysTick_Run(unsigned int msec);
extern int SysTick_Check_Timeout(void);
extern unsigned int SysTick_Get_Time(void);
extern unsigned int SysTick_Get_Load_Time(void);
extern void SysTick_Stop(void);

/* ============================= Led.c ============================= */

extern void LED_Init(void);
extern void LED_On(void);
extern void LED_Off(void);

/* ============================ Clock.c ============================ */

extern void Clock_Init(void);

/* ============================= Key.c ============================= */

extern void Key_Poll_Init(void);
extern int Key_Get_Pressed(void);
extern void Key_Wait_Key_Released(void);
extern void Key_Wait_Key_Pressed(void);

/* ============================ Timer.c ============================ */

extern void TIM2_Delay(int time);
extern void TIM2_Stopwatch_Start(void);
extern unsigned int TIM2_Stopwatch_Stop(void);
extern void TIM4_Repeat(int time);
extern int TIM4_Check_Timeout(void);
extern void TIM4_Stop(void);
extern void TIM4_Change_Value(int time);
extern void TIM4_Repeat_Interrupt_Enable(int en, int time);
extern void TIM3_Out_Init(void);
extern void TIM3_Out_Freq_Generation(unsigned short freq);
extern void TIM3_Out_Stop(void);

extern volatile int g_lockout_done;

/* ============================= Spi.c ============================= */

extern void SPI1_SC16IS752_Init(unsigned int div);
extern void SPI1_SC16IS752_Write_Reg(unsigned int addr, unsigned int data);
extern unsigned int SPI1_SC16IS752_Read_Reg(unsigned int addr);
extern void SPI1_SC16IS752_Config_GPIO(unsigned int config);
extern void SPI1_SC16IS752_Write_GPIO(unsigned int data);
extern unsigned int SPI1_SC16IS752_Read_GPIO(void);

/* ============================= I2c.c ============================= */

extern void I2C1_Init(unsigned int freq);
extern void I2C1_Send_Byte(unsigned char i2c_addr, unsigned char data);

/* ============================= Lcd.c ============================= */

extern void LCD_Init(void);
extern void LCD_Clear(void);
extern void LCD_SetCursor(int row, int col);
extern void LCD_Print(int row, int col, char *str);
extern void LCD_Command(unsigned char cmd);
extern void LCD_Data(unsigned char data);

/* =========================== Keypad.c ============================ */

extern void Keypad_Init(void);
extern char Keypad_Scan(void);
extern void Keypad_Debug_ScanAll(unsigned int col_vals[4]);

/* ============================ Servo.c ============================ */

extern void Servo_Init(void);
extern void Servo_SetAngle(int angle);
