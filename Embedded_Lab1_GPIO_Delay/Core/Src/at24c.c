#include "at24c.h"

uint8_t at24c_Full_Check(void)
{
    uint16_t addr;
    uint8_t testValue;  // Test value to write
    uint8_t readValue;

    // Iterate through all available addresses
    for (addr = 0; addr <= EE_TYPE; addr++)
    {
        testValue = addr % 256; // Set test value as increasing integer (mod 256 for 8-bit range)

        // Write the test value to the current address
        at24c_WriteOneByte(addr, testValue);
        HAL_Delay(5);  // Ensure the write cycle is completed

        // Read back the value from the current address
        readValue = at24c_ReadOneByte(addr);

        // Verify the value matches the written value
        if (readValue != testValue)
        {
            return 1;  // Return error if the values do not match
        }
    }

    return 0;  // Return 0 if all addresses pass the check
}

void at24c_WriteFloat(uint16_t WriteAddr, float data)
{
    uint8_t *pData = (uint8_t*)&data;
    for (uint8_t i = 0; i < sizeof(float); i++)
    {
        at24c_WriteOneByte(WriteAddr + i, *(pData + i));
        HAL_Delay(5);  // Ensure write cycle completes
    }
}

float at24c_ReadFloat(uint16_t ReadAddr)
{
    float data;
    uint8_t *pData = (uint8_t*)&data;
    for (uint8_t i = 0; i < sizeof(float); i++)
    {
        *(pData + i) = at24c_ReadOneByte(ReadAddr + i);
    }
    return data;
}

void at24c_WriteInt(uint16_t WriteAddr, int data)
{
    uint8_t *pData = (uint8_t*)&data;
    for (uint8_t i = 0; i < sizeof(int); i++)
    {
        at24c_WriteOneByte(WriteAddr + i, *(pData + i));
        HAL_Delay(5);  // Ensure write cycle completes
    }
}

int at24c_ReadInt(uint16_t ReadAddr)
{
    int data;
    uint8_t *pData = (uint8_t*)&data;
    for (uint8_t i = 0; i < sizeof(int); i++)
    {
        *(pData + i) = at24c_ReadOneByte(ReadAddr + i);
    }
    return data;
}

void at24c_WriteString(uint16_t WriteAddr, const char *str)
{
    while (*str)
    {
        at24c_WriteOneByte(WriteAddr++, *str++);
        HAL_Delay(5);  // Ensure write cycle completes
    }
    at24c_WriteOneByte(WriteAddr, '\0');  // Write null terminator
    HAL_Delay(5);
}

void at24c_ReadString(uint16_t ReadAddr, char *buffer, uint16_t maxLength)
{
    uint16_t i = 0;
    do
    {
        buffer[i] = at24c_ReadOneByte(ReadAddr + i);
    } while (buffer[i++] != '\0' && i < maxLength);

    buffer[maxLength - 1] = '\0';  // Ensure null termination
}
