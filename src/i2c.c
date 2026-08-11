#include "I2C.h"

void I2C_StartInterrupt(I2C_TypeDef_t *i2c)
{
    i2c->CR |= I2C_INTR_EN;
}

void I2C_StopInterrupt(I2C_TypeDef_t *i2c)
{
    i2c->CR &= ~I2C_INTR_EN;
}

void I2C_WaitDone(I2C_TypeDef_t *i2c)
{
    delay_ms(10);
}

void I2C_Start(I2C_TypeDef_t *i2c)
{
    i2c->CR = (i2c->CR & ~0x1E) | I2C_CMD_START;
    I2C_WaitDone(i2c);
}

void I2C_Stop(I2C_TypeDef_t *i2c)
{
    i2c->CR = (i2c->CR & ~0x1E) | I2C_CMD_STOP;
    I2C_WaitDone(i2c);
}

void I2C_WriteByte(I2C_TypeDef_t *i2c, uint8_t data)
{
    i2c->DR = (uint32_t)data;
    i2c->CR = (i2c->CR & ~0x1E) | I2C_CMD_WRITE;
    I2C_WaitDone(i2c);
}

uint8_t I2C_ReadByte(I2C_TypeDef_t *i2c)
{
    i2c->CR = (i2c->CR & ~0x1E) | I2C_CMD_READ;
    I2C_WaitDone(i2c);
    return (uint8_t)(i2c->DR);
}

void I2C_Write(I2C_TypeDef_t *i2c, uint8_t slave_addr,
               uint8_t *data, uint32_t len)
{
    uint32_t i;

    i2c->ADDR = (uint32_t)slave_addr;

    I2C_Start(i2c);

    I2C_WriteByte(i2c, (slave_addr << 1) | 0x00);

    for (i = 0; i < len; i++) {
        I2C_WriteByte(i2c, data[i]);
    }

    I2C_Stop(i2c);
}
