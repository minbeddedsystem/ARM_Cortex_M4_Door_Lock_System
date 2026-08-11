#include "LCD.h"
#include "../../driver/FND/FND.h"

static void LCD_SendNibble(I2C_TypeDef_t *i2c, uint8_t nibble, uint8_t rs)
{
    uint8_t data;

    data = (nibble & 0xF0) | LCD_BL | LCD_EN | (rs ? LCD_RS : 0);
    I2C_Write(i2c, LCD_I2C_ADDR, &data, 1);
    delay_ms(5);

    data = (nibble & 0xF0) | LCD_BL | (rs ? LCD_RS : 0);
    I2C_Write(i2c, LCD_I2C_ADDR, &data, 1);
    delay_ms(5);
}

static void LCD_Send(I2C_TypeDef_t *i2c, uint8_t byte, uint8_t rs)
{
    LCD_SendNibble(i2c, byte & 0xF0, rs);
    LCD_SendNibble(i2c, (byte << 4) & 0xF0, rs);
}

static void LCD_SendCmd(I2C_TypeDef_t *i2c, uint8_t cmd)
{
    LCD_Send(i2c, cmd, 0);
}

static void LCD_SendData(I2C_TypeDef_t *i2c, uint8_t data)
{
    LCD_Send(i2c, data, 1);
}

void LCD_Init(I2C_TypeDef_t *i2c)
{
    delay_ms(100);

    LCD_SendNibble(i2c, 0x30, 0);
    delay_ms(10);
    LCD_SendNibble(i2c, 0x30, 0);
    delay_ms(5);
    LCD_SendNibble(i2c, 0x30, 0);
    delay_ms(5);
    LCD_SendNibble(i2c, 0x20, 0);
    delay_ms(5);

    LCD_SendCmd(i2c, LCD_CMD_FUNC_SET);
    delay_ms(5);
    LCD_SendCmd(i2c, LCD_CMD_DISPLAY_ON);
    delay_ms(5);
    LCD_SendCmd(i2c, LCD_CMD_CLEAR);
    delay_ms(5);
    LCD_SendCmd(i2c, LCD_CMD_ENTRY);
    delay_ms(5);
}

void LCD_Clear(I2C_TypeDef_t *i2c)
{
    LCD_SendCmd(i2c, LCD_CMD_CLEAR);
    delay_ms(5);
}

void LCD_SetCursor(I2C_TypeDef_t *i2c, uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? LCD_CMD_LINE1 : LCD_CMD_LINE2;
    LCD_SendCmd(i2c, addr + col);
}

void LCD_WriteChar(I2C_TypeDef_t *i2c, char c)
{
    LCD_SendData(i2c, (uint8_t)c);
}

void LCD_WriteString(I2C_TypeDef_t *i2c, char *str)
{
    while (*str) {
        LCD_WriteChar(i2c, *str++);
    }
}

void LCD_WriteInt(I2C_TypeDef_t *i2c, uint32_t num)
{
    char buf[11];
    int i = 0;
    if (num == 0) {
        LCD_WriteChar(i2c, '0');
        return;
    }
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i > 0) {
        LCD_WriteChar(i2c, buf[--i]);
    }
}
