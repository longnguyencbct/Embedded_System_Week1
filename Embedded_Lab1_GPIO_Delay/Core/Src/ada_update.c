#include "ada_update.h"

uint8_t count_adc = 0;

void test_Adc() {
    count_adc = (count_adc + 1) % 200;
	test_Esp();
	lightProcess();

    if (count_adc == 0) {
        // Read sensor values

        sensor_Read();

        // Temperature
        lcd_ShowStr(10, 180, "Temperature:", RED, BLACK, 16, 0);
        lcd_ShowFloatNum(130, 180, sensor_GetTemperature(), 4, RED, BLACK, 16);
        int tempInt = (int)(sensor_GetTemperature() * 100); // Scale float to int (2 decimal places)
        char tempStr[30];
        snprintf(tempStr, sizeof(tempStr), "!TEMPERATURE:%d.%02d#\n", tempInt / 100, tempInt % 100);
        uart_EspSendBytes(tempStr, strlen(tempStr));
    }
}

