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
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"
#include "uart.h"
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
#include "touch.h"
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
#define INIT 0
#define PLAYING 1
#define GAME_OVER 2

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

int game_state = INIT;
int direction = UP; // Mặc định hướng ban đầu là UP
int snake_length = 3; // Độ dài mặc định của rắn

typedef struct {
    int x;
    int y;
} Point;

Point snake[100]; // Tối đa 100 phần tử rắn
Point food;       // Tọa độ thức ăn

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void touchProcess();
void updateGame();
void drawGameOverScreen();
uint8_t isButtonClear();
uint8_t isTouchedStartButton();
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
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // Hiển thị giao diện khởi đầu
  drawStartScreen();//test
 while (1)
  {
	touch_Scan(); // Quét màn hình cảm ứng
	if (flag_timer2) {
	  flag_timer2 = 0;

	  switch (game_state) {
		case INIT: {
		  if (isTouchedStartButton()) {
			  initializeGame(); // Khởi tạo trò chơi
			  lcd_Clear(BLACK);
			  drawGameFrame();
			  drawSnakeAndFood(); // Vẽ rắn và thức ăn
			  game_state = PLAYING;
		  }
		  break;
		}

		case PLAYING: {
		  handleNavigationButtons(); // Xử lý điều khiển
		  updateSnake();             // Cập nhật trạng thái rắn
		  drawSnakeAndFood();        // Vẽ lại rắn và thức ăn
		  break;
		}

		case GAME_OVER: {
		  drawGameOverScreen(); // Hiển thị giao diện Game Over
		  if (isTouchedStartButton()) {
			game_state = INIT; // Quay về trạng thái khởi đầu
			drawStartScreen();
		  }
		  break;
		}

		default:
			game_state = INIT; // Quay về trạng thái khởi đầu
		  break;
	  }
	}
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
	  timer_init();
	  button_init();
	  lcd_init();
	  touch_init();
	  setTimer2(50);
}

uint8_t count_led_debug = 0;

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

void initializeGame() {
    // Khởi tạo rắn với 3 điểm liên tiếp
    snake[0] = (Point){10, 5};
    snake[1] = (Point){9, 5};
    snake[2] = (Point){8, 5};

    snake_length = 3;
    direction = RIGHT;

    // Sinh thức ăn ngẫu nhiên, đảm bảo không trùng với rắn
    generateRandomFood();
}

void generateRandomFood() {
    do {
        food.x = rand() % 20; // Khung 20x20
        food.y = rand() % 20;
    } while (isFoodOnSnake(food));
}


int isFoodOnSnake(Point food) {
    for (int i = 0; i < snake_length; i++) {
        if (snake[i].x == food.x && snake[i].y == food.y) {
            return 1;
        }
    }
    return 0;
}

void updateSnake() {
	// Tính toán đầu mới dựa trên hướng
	Point newHead = snake[0];
	switch (direction) {
		case UP:    newHead.y--; break;
		case DOWN:  newHead.y++; break;
		case LEFT:  newHead.x--; break;
		case RIGHT: newHead.x++; break;
	}

	// Kiểm tra va chạm tường hoặc thân
	if (newHead.x < 0 || newHead.y < 0 || newHead.x >= 20 || newHead.y >= 20 || isFoodOnSnake(newHead)) {
		game_state = GAME_OVER;
		return;
	}

	// Thêm đầu mới
	for (int i = snake_length; i > 0; i--) {
		snake[i] = snake[i - 1];
	}
	snake[0] = newHead;

	// Kiểm tra ăn thức ăn
	if (newHead.x == food.x && newHead.y == food.y) {
		snake_length++; // Tăng chiều dài rắn
		generateRandomFood(); // Sinh thức ăn mới
	} else {
		// Nếu không ăn thức ăn, xóa đuôi
		snake[snake_length] = (Point){-1, -1}; // Đánh dấu phần tử trống
	}
}


void drawSnakeAndFood() {
    // Vẽ rắn
    for (int i = 0; i < snake_length; i++) {
        lcd_DrawPoint(snake[i].x * 10, snake[i].y * 10, GREEN); // Mỗi ô là 10x10 pixel
    }

    // Vẽ thức ăn
    lcd_DrawPoint(food.x * 10, food.y * 10, RED);
}

uint8_t isTouchedStartButton() {
    if (!touch_IsTouched()) return 0;
    uint16_t x = touch_GetX();
    uint16_t y = touch_GetY();
    return (x >= 125 && x <= 225 && y >= 210 && y <= 240);
}


// Hàm xử lý các nút điều hướng
void handleNavigationButtons() {
    if (!touch_IsTouched()) return;
    uint16_t x = touch_GetX();
    uint16_t y = touch_GetY();

    if (x >= 150 && x <= 200 && y >= 215 && y <= 245) {
        if (direction != DOWN) direction = UP; // Ngăn quay từ DOWN sang UP
    } else if (x >= 150 && x <= 200 && y >= 255 && y <= 285) {
        if (direction != UP) direction = DOWN; // Ngăn quay từ UP sang DOWN
    } else if (x >= 100 && x <= 150 && y >= 255 && y <= 285) {
        if (direction != RIGHT) direction = LEFT; // Ngăn quay từ RIGHT sang LEFT
    } else if (x >= 200 && x <= 250 && y >= 255 && y <= 285) {
        if (direction != LEFT) direction = RIGHT; // Ngăn quay từ LEFT sang RIGHT
    }
}

// Hiển thị màn hình Game Over
void drawGameOverScreen() {
    lcd_Clear(BLACK); // Xóa màn hình
    lcd_ShowStr(100, 100, "GAME OVER", RED, BLACK, 24, 1); // Hiển thị thông báo
    lcd_ShowStr(80, 150, "Tap Start to retry", WHITE, BLACK, 16, 1); // Thông báo cách chơi lại
}

void drawStartScreen() {
    lcd_Clear(BLACK); // Xóa màn hình với màu đen
    drawGameFrame();  // Vẽ khung trò chơi
    drawNavigationButtons(); // Vẽ các nút điều hướng
    drawStartButton(); // Vẽ nút Start
}

void drawStartButton() {
	// Vẽ nút "Start" với màu xanh dương nhạt
	lcd_Fill(125, 210, 225, 240, GBLUE);
	// Hiển thị chữ "Start" với màu trắng
	lcd_ShowStr(150, 220, "Start", WHITE, GBLUE, 24, 1);
}

void drawGameFrame() {
	// Vẽ khung trò chơi với màu trắng
	lcd_DrawRectangle(75, 10, 275, 210, WHITE);
}

void drawNavigationButtons() {
	// Nút UP
	lcd_Fill(150, 215, 200, 245, GBLUE);
	lcd_ShowStr(165, 225, "UP", WHITE, GBLUE, 16, 1);
	// Nút DOWN
	lcd_Fill(150, 255, 200, 285, GBLUE);
	lcd_ShowStr(155, 265, "DOWN", WHITE, GBLUE, 16, 1);
	// Nút LEFT
	lcd_Fill(100, 255, 150, 285, GBLUE);
	lcd_ShowStr(115, 265, "LEFT", WHITE, GBLUE, 16, 1);
	// Nút RIGHT
	lcd_Fill(200, 255, 250, 285, GBLUE);
	lcd_ShowStr(215, 265, "RIGHT", WHITE, GBLUE, 16, 1);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
