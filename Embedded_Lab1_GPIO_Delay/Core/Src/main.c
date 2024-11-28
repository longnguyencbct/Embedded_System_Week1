/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "uart.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"
#include "light_control.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float power_samples[100] = {0}; // Mảng lưu giá trị công suất
uint8_t sampling_period = 1;    // Chu kỳ lấy mẫu (1 giây mặc định)
uint8_t grid_split =10;
uint16_t time_range = 100;      // Chi�?u dài trục OX (100 đơn vị th�?i gian)
float max_power = 300.0;         // Giá trị tối đa trục OY (10 mW mặc định)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void test_Buzzer();
void test_Adc();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_TIM13_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 lcd_Clear(BLACK);
  while (1)
  {
	  while(!flag_timer2);
	  flag_timer2 = 0;
	  button_Scan();
	  test_LedDebug();
	  test_Esp();
	  lightProcess();
	  test_Adc_Uart();
	  test_Buzzer();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
    timer_init();             // Khởi tạo Timer
    button_init();            // Khởi tạo nút nhấn
    lcd_init();               // Khởi tạo LCD
    sensor_init();            // Khởi tạo cảm biến
    buzzer_init();            // Khởi tạo Buzzer
    uart_init_rs232();        // Khởi tạo UART
    ds3231_init();            // Khởi tạo RTC
    setTimer2(50);            // Chu kỳ hệ thống 50ms
}

uint8_t count_led_debug = 0;

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

uint8_t isButtonE()
{
    if (button_count[12] >= 10)
        return 1;
    else
        return 0;
}

uint8_t buttonE_prev_state = 0; // Trạng thái trước đó của nút E
uint8_t display_adc_info = 1; // 1: Hiển thị, 0: Không hiển thị
void Check_Toggle_LCD_Display(){
// Kiểm tra trạng thái nút E (nút 13)
	if (isButtonE()) {
		if (buttonE_prev_state == 0) {
			// Toggle trạng thái hiển thị khi nút E được nhấn
			display_adc_info = !display_adc_info;
		}
		buttonE_prev_state = 1; // Cập nhật trạng thái nút E
	} else {
		buttonE_prev_state = 0; // Cập nhật trạng thái nút E khi thả
	}
}

uint8_t isButtonUp()
{
    if (button_count[3] == 1)
        return 1;
    else
        return 0;
}

uint8_t isButtonDown()
{
    if (button_count[7] == 1)
        return 1;
    else
        return 0;
}

uint8_t isButtonRight()
{
    if (button_count[11] == 1)
        return 1;
    else
        return 0;
}

void new_show_num(float value, int pos){
    char debug_msg[7];
    sprintf(debug_msg, "%.2f", value); // Correctly format float value
    lcd_ShowStr(150, pos, debug_msg, RED, BLACK, 16, 0);
}

void debug_lcd(float value, char* str, int pos) {
    char debug_msg[100];

    // Format the debug message
    sprintf(debug_msg, "%s=%.2f", str, value); // Correctly format float value

    // Calculate y-coordinate for the debug message on the LCD
    int y_position = 230 + pos * 20; // Add spacing for each debug line

    // Display debug message on the LCD
    lcd_ShowStr(10, y_position, debug_msg, YELLOW, BLACK, 16, 0);

    // Send debug message over UART
    uart_Rs232SendString((uint8_t *)debug_msg);
    uart_Rs232SendString((uint8_t *)"\n");
}


uint8_t count_adc = 0;

void test_Adc_Uart() {
    count_adc = (count_adc + 1) % 20;
    if (count_adc == 0) {
        sensor_Read();  // �?�?c dữ liệu từ cảm biến

        // �?iện áp, dòng điện, công suất tiêu thụ
        float voltage = sensor_GetVoltage();
        float current = ( sensor_GetCurrent());
        int power = (int)(voltage * current);
        store_power_data(power);
        // Cập nhật biểu đồ trên LCD (vẫn luôn cập nhật)
        plot_power_chart();

        // �?ộ sáng
        uint16_t light = sensor_GetLight();
        const char *light_status = (light > 2000) ? "Strong" : "Weak";

        // �?ộ ẩm (tính phần trăm)
        uint16_t humidity_adc = sensor_GetPotentiometer();
        uint16_t humidity_percent = (humidity_adc * 100) / 4095;

        // Nhiệt độ
        float temperature = sensor_GetTemperature();

        // Th�?i gian thực
        char time_str[10];
        ds3231_GetTime(time_str); // Lấy th�?i gian từ RTC

        // Hiển thị lên LCD nếu trạng thái hiển thị bật
        Check_Toggle_LCD_Display();
        if (display_adc_info) {
            lcd_ShowStr(10, 50, "Voltage (V):", RED, BLACK, 16, 0);
            lcd_ShowFloatNum(150, 50, voltage, 4, RED, BLACK, 16);

            lcd_ShowStr(10, 70, "Current (mA):", RED, BLACK, 16, 0);
//            lcd_ShowFloatNum(150, 70, disp_current, 6, RED, BLACK, 16);
            new_show_num(current, 70);

            lcd_ShowStr(10, 90, "Power (mW):", RED, BLACK, 16, 0);
            lcd_ShowFloatNum(150, 90, power, 6, RED, BLACK, 16);

            lcd_ShowStr(10, 110, "Light:", RED, BLACK, 16, 0);
            lcd_ShowStr(150, 110, light_status, RED, BLACK, 16, 0);

            lcd_ShowStr(10, 130, "Humidity (%):", RED, BLACK, 16, 0);
            lcd_ShowIntNum(150, 130, humidity_percent, 4, RED, BLACK, 16);

            lcd_ShowStr(10, 150, "Temperature (C):", RED, BLACK, 16, 0);
            lcd_ShowFloatNum(150, 150, temperature, 5, RED, BLACK, 16);

            lcd_ShowStr(10, 170, "Time:", RED, BLACK, 16, 0);
            lcd_ShowStr(150, 170, time_str, RED, BLACK, 16, 0);
        }

        // Gửi dữ liệu qua UART (vẫn luôn gửi)
//        char msg[200];
//        sprintf(msg, "Voltage: %.2f V\nCurrent: %.2f mA\nPower: %d mW\nLight: %s\nHumidity: %d %%\nTemperature: %.2f C\nTime: %s\n",
//                voltage, current, power, light_status, humidity_percent, temperature, time_str);
//        uart_Rs232SendString((uint8_t *)msg);
//        uart_Rs232SendString((uint8_t *)"-----------------------\n");
//
//        // Kiểm tra ngưỡng độ ẩm và cảnh báo nếu cần
//        if (humidity_percent > 70) {
//            buzzer_SetVolume(50); // Kích hoạt báo động
//            uart_Rs232SendString((uint8_t *)"WARNING: High Humidity!\n");
//        } else {
//            buzzer_SetVolume(0); // Tắt báo động
//        }
//        uart_Rs232SendString((uint8_t *)"o");
    }
}


void store_power_data(int power) {
    // Shift all elements one step to the left
    for (int i = 0; i < time_range - 1; i++) {
        power_samples[i] = power_samples[i + 1];
    }
    // Store the new power value at the last position
    power_samples[time_range - 1] = power;
}


int get_digit_count(int number) {
    int count = 0;
    if (number == 0) return 1; // Trư�?ng hợp đặc biệt, số 0 có 1 chữ số
    while (number != 0) {
        number /= 10;
        count++;
    }
    return count;
}


void plot_power_chart() {
    // Xóa vùng biểu đồ
    lcd_Fill(10, 10, 210, 210, BLACK);

    // Vẽ grid d�?c (OX) theo grid_split
    for (int i = 0; i <= grid_split; i++) {
        int x = 10 + (200 * i / grid_split); // Tính t�?a độ x dựa trên grid_split
        lcd_DrawLine(x, 10, x, 210, LIGHTGRAY);
    }

    // Vẽ grid ngang (OY) theo grid_split
    for (int i = 0; i <= grid_split; i++) {
        int y = 210 - (200 * i / grid_split); // Tính t�?a độ y dựa trên grid_split
        lcd_DrawLine(10, y, 210, y, LIGHTGRAY);
    }

    // Vẽ các nhãn trục OY
    for (int i = 0; i <= grid_split; i++) {
        int label = (int)(i * max_power / grid_split); // Giá trị nhãn trục OY
        int y = 210 - (200 * i / grid_split);         // T�?a độ y
        int len = get_digit_count(label);             // Số chữ số của `label`
        lcd_ShowIntNum(0, y, label, len, WHITE, BLACK, 16);
    }

    // Vẽ các nhãn trục OX
    for (int i = 0; i <= grid_split; i++) {
        int label = (int)(time_range - (i * time_range / grid_split)); // Giá trị nhãn trục OX
        int x = 10 + (200 * i / grid_split);                          // Tính t�?a độ x
        int len = get_digit_count(label);                             // Số chữ số của `label`
        lcd_ShowIntNum(x, 215, label, len, WHITE, BLACK, 16);
    }

    // Vẽ đư�?ng biểu diễn công suất
    for (int i = 0; i < time_range - 1; i++) {
        // Chuyển đổi giá trị từ `power_samples` thành t�?a độ pixel
        int x1 = 10 + (200 * i / time_range);
        int y1 = 210 - (int)((200 * power_samples[i]) / max_power);
        int x2 = 10 + (200 * (i + 1) / time_range);
        int y2 = 210 - (int)((200 * power_samples[i + 1]) / max_power);

        // �?ảm bảo t�?a độ y1 và y2 nằm trong phạm vi hợp lệ
        if (y1 < 10) y1 = 10;
        if (y1 > 210) y1 = 210;
        if (y2 < 10) y2 = 10;
        if (y2 > 210) y2 = 210;

        // Vẽ đư�?ng nối giữa các điểm
        lcd_DrawLine(x1, y1, x2, y2, RED);
    }
}













void test_Buzzer(){
	if(isButtonUp()){
		buzzer_SetVolume(50);
	}

	if(isButtonDown()){
		buzzer_SetVolume(0);
	}

	if(isButtonRight()){
		buzzer_SetVolume(25);
	}

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
