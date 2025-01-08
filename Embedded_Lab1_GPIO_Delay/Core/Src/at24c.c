#include "at24c.h"
#include "lcd.h"

uint8_t at24c_Full_Check(void)
{
	uint32_t addr;
    uint8_t testValue;  // Test value to write
    uint8_t readValue;

    for (addr = 0; addr <= EE_TYPE; addr++)
    {
        lcd_Clear(BLACK); // Clear the screen for each iteration

        testValue = addr % 256; // Set test value (mod 256 for 8-bit range)

        // Show debug info
        char debug_str[50];

        sprintf(debug_str, "== ADDRESS %d ==", addr);
		lcd_ShowStr(10, 50, debug_str, WHITE, BLACK, 16, 1);

        // Write the test value
        at24c_WriteOneByte(addr, testValue);
        sprintf(debug_str, "%d Write: %d", addr, testValue);
        lcd_ShowStr(10, 70, debug_str, WHITE, BLACK, 16, 1);

        // Read back the value
        readValue = at24c_ReadOneByte(addr);
        sprintf(debug_str, "%d Read: %d", addr, readValue);
        lcd_ShowStr(10, 90, debug_str, WHITE, BLACK, 16, 1);

        // Verify the value
        if (readValue != testValue)
        {
            return 1; // Error
        }
//        HAL_Delay(100);
    }

    return 0; // Success
}

uint8_t at24c_ReadOneByte(uint16_t ReadAddr)
{
    uint8_t temp = 0;
    HAL_I2C_Mem_Read(&hi2c1, 0xA0, ReadAddr, I2C_MEMADD_SIZE_16BIT, &temp, 1, 10);
    return temp;
}

void at24c_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite)
{
	if (HAL_I2C_Mem_Write(&hi2c1, 0xA0, WriteAddr, I2C_MEMADD_SIZE_16BIT, &DataToWrite, 1, 100) != HAL_OK)
	{
		lcd_ShowStr(10, 120, "Write Error", WHITE, BLACK, 16, 1);
	}
    HAL_Delay(5);
}

void at24c_Read(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead)
{
    while (NumToRead)
    {
        *pBuffer++ = at24c_ReadOneByte(ReadAddr++);
        NumToRead--;
    }
}

void at24c_Write(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
    while (NumToWrite--)
    {
        at24c_WriteOneByte(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}

void at24c_init(void)
{
    at24c_Check();
}

uint8_t at24c_Check(void)
{
    uint8_t temp;
    temp = at24c_ReadOneByte(65535);
    if (temp == 0x55) return 0;
    else
    {
        at24c_WriteOneByte(65535, 0x55);
        temp = at24c_ReadOneByte(65535);
        if (temp == 0x55) return 0;
    }
    return 1;
}
