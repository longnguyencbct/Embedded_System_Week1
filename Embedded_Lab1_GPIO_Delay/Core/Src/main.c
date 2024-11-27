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
int snake_length = 3; // �?ộ dài mặc định của rắn

typedef struct {
    int x;
    int y;
} Point;

Point snake[100]; // Tối đa 100 phần tử rắn
Point food;       // T�?a độ thức ăn

int gameFrameX1 = 10;
int gameFrameY1 = 0;
int gameFrameX2 = 230;
int gameFrameY2 = 220;

// T�?a độ nút
int buttonUpX1 = 100;
int buttonUpY1 = 230;
int buttonUpX2 = 150;
int buttonUpY2 = 260;

int buttonDownX1 = 100;
int buttonDownY1 = 290;
int buttonDownX2 = 150;
int buttonDownY2 = 320;

int buttonLeftX1 = 50;
int buttonLeftY1 = 260;
int buttonLeftX2 = 100;
int buttonLeftY2 = 290;

int buttonRightX1 = 150;
int buttonRightY1 = 260;
int buttonRightX2 = 200;
int buttonRightY2 = 290;

int buttonStartX1 = 70;
int buttonStartY1 = 95;
int buttonStartX2 = 170;
int buttonStartY2 = 125;

int buttonReturnX1 = 80;
int buttonReturnY1 = 200;
int buttonReturnX2 = 160;
int buttonReturnY2 = 230;


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
uint8_t isTouchedReturnButton();
uint8_t isHeadOnFood(Point);
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
  uint8_t CMD_RDX=0XD0;
  uint8_t CMD_RDY=0X90;
 while (1)
  {
	touch_Scan(); // Quét màn hình cảm ứng
	if (flag_timer2) {
		flag_timer2 = 0;

	  switch (game_state) {
	  case INIT: {
	      drawStartScreen(); // Ensure the start screen, including buttons, is drawn
	      if (isTouchedStartButton()) {
	          initializeGame();        // Khởi tạo trò chơi
	          lcd_Clear(BLACK);        // Clear the screen for PLAYING state
	          drawGameFrame();         // Draw game frame
	          drawSnakeAndFood();      // Vẽ rắn và thức ăn
	          game_state = PLAYING;    // Switch to PLAYING state
	      }
	      break;
	  }


		case PLAYING: {
			drawPlayScreen();
		  handleNavigationButtons(); // Xử lý đi�?u khiển
		  updateSnake();             // Cập nhật trạng thái rắn
		  drawSnakeAndFood();        // Vẽ lại rắn và thức ăn
		  break;
		}

		case GAME_OVER: {
		    drawGameOverScreen(); // Display the Game Over screen
		    if (isTouchedStartButton()) {
		        initializeGame();  // Restart the game
		        game_state = PLAYING; // Switch to the PLAYING state
		    } else if (isTouchedReturnButton()) {
		        game_state = INIT; // Transition back to INIT state
		        drawStartScreen(); // Display the start screen
		    }
		    break;
		}


		default:
			game_state = INIT; // Quay v�? trạng thái khởi đầu
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
	  setTimer2(500);
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
    int min_x = gameFrameX1;
    int min_y = gameFrameY1;
    int max_x = gameFrameX2 - 5;  // Account for food size
    int max_y = gameFrameY2 - 5;  // Account for food size

    do {
        food.x = (rand() % ((max_x - min_x) / 5)) * 5 + min_x; // Snap to 5-pixel grid
        food.y = (rand() % ((max_y - min_y) / 5)) * 5 + min_y; // Snap to 5-pixel grid
    } while (isFoodOnSnake(food)); // Ensure no overlap with the snake
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
    // Calculate the new head position based on the direction
    Point newHead = snake[0];
    switch (direction) {
        case UP:    newHead.y -= 5; break;
        case DOWN:  newHead.y += 5; break;
        case LEFT:  newHead.x -= 5; break;
        case RIGHT: newHead.x += 5; break;
    }

    // Check for collision with the frame boundaries or self
    if (newHead.x < gameFrameX1 || newHead.y < gameFrameY1 ||
        newHead.x >= gameFrameX2 || newHead.y >= gameFrameY2 || isFoodOnSnake(newHead)) {
        game_state = GAME_OVER;
        return;
    }

    // Shift the snake's body to make room for the new head
    for (int i = snake_length; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = newHead;

    // Check if the snake's head overlaps with the food
    if (isHeadOnFood(newHead)) {
        snake_length++;  // Grow the snake
        generateRandomFood();  // Place new food
    } else {
        // Clear the last segment (snake tail) if no food is eaten
        snake[snake_length] = (Point){-1, -1};
    }
}


uint8_t isHeadOnFood(Point head) {
    // Check if the head overlaps with the food
    return !(head.x + 5 <= food.x || food.x + 5 <= head.x ||
             head.y + 5 <= food.y || food.y + 5 <= head.y);
}



void drawSnakeAndFood() {
    // Clear the previous frame inside the game area
//    lcd_Fill(gameFrameX1, gameFrameY1, gameFrameX2, gameFrameY2, BLACK);

    // Draw the snake
    for (int i = 0; i < snake_length; i++) {
        lcd_Fill(snake[i].x, snake[i].y,
                 snake[i].x + 5, snake[i].y + 5, GREEN); // Render 5x5 blocks
    }

    // Draw the food
    lcd_Fill(food.x, food.y,
             food.x + 5, food.y + 5, RED); // Render 5x5 blocks
}



uint8_t isTouchedStartButton() {
    if (!touch_IsTouched()) return 0;
    uint16_t x = touch_GetX();
    uint16_t y = touch_GetY();
    return (x >= buttonStartX1 && x <= buttonStartX2 && y >= buttonStartY1 && y <= buttonStartY2);
}



// Hàm xử lý các nút đi�?u hướng
void handleNavigationButtons() {
    if (!touch_IsTouched()) return;
    uint16_t x = touch_GetX();
    uint16_t y = touch_GetY();

    if (x >= buttonUpX1 && x <= buttonUpX2 && y >= buttonUpY1 && y <= buttonUpY2) {
        if (direction != DOWN) direction = UP;
    } else if (x >= buttonDownX1 && x <= buttonDownX2 && y >= buttonDownY1 && y <= buttonDownY2) {
        if (direction != UP) direction = DOWN;
    } else if (x >= buttonLeftX1 && x <= buttonLeftX2 && y >= buttonLeftY1 && y <= buttonLeftY2) {
        if (direction != RIGHT) direction = LEFT;
    } else if (x >= buttonRightX1 && x <= buttonRightX2 && y >= buttonRightY1 && y <= buttonRightY2) {
        if (direction != LEFT) direction = RIGHT;
    }
}

uint8_t isTouchedReturnButton() {
    if (!touch_IsTouched()) return 0; // Check if the screen is touched
    uint16_t x = touch_GetX();
    uint16_t y = touch_GetY();
    return (x >= buttonReturnX1 && x <= buttonReturnX2 && y >= buttonReturnY1 && y <= buttonReturnY2);
}


// Hiển thị màn hình Game Over
void drawGameOverScreen() {
    lcd_Clear(BLACK); // Clear the screen
    lcd_ShowStr(70, 100, "GAME OVER", RED, BLACK, 24, 1); // Display "Game Over"
    lcd_ShowStr(50, 150, "Tap Start to retry", WHITE, BLACK, 16, 1); // Retry message

    // Draw the Return button
    lcd_Fill(buttonReturnX1, buttonReturnY1, buttonReturnX2, buttonReturnY2, GBLUE);
    lcd_ShowStr(buttonReturnX1 + 10, buttonReturnY1 + 5, "Return", WHITE, GBLUE, 16, 1);
}


void drawStartScreen() {
    lcd_Clear(BLACK);           // Xóa màn hình với màu đen
    drawGameFrame();            // Vẽ khung trò chơi
//    drawNavigationButtons();    // Vẽ các nút điều hướng
    drawStartButton();          // Vẽ nút Start
}

void drawPlayScreen(){
    lcd_Clear(BLACK);           // Xóa màn hình với màu đen
    drawGameFrame();
	drawNavigationButtons();    // Vẽ các nút điều hướng
}

void drawStartButton() {
    lcd_Fill(buttonStartX1, buttonStartY1, buttonStartX2, buttonStartY2, GBLUE);
    lcd_ShowStr(buttonStartX1 + 25, buttonStartY1 + 5, "Start", WHITE, GBLUE, 24, 1);
}


void drawGameFrame() {
    lcd_DrawRectangle(gameFrameX1, gameFrameY1, gameFrameX2, gameFrameY2, WHITE);
}


void drawNavigationButtons() {
    // Nút UP
    lcd_Fill(buttonUpX1, buttonUpY1, buttonUpX2, buttonUpY2, GBLUE);
    lcd_ShowStr(buttonUpX1 + 15, buttonUpY1 + 5, "UP", WHITE, GBLUE, 16, 1);

    // Nút DOWN
    lcd_Fill(buttonDownX1, buttonDownY1, buttonDownX2, buttonDownY2, GBLUE);
    lcd_ShowStr(buttonDownX1 + 5, buttonDownY1 + 5, "DOWN", WHITE, GBLUE, 16, 1);

    // Nút LEFT
    lcd_Fill(buttonLeftX1, buttonLeftY1, buttonLeftX2, buttonLeftY2, GBLUE);
    lcd_ShowStr(buttonLeftX1 + 10, buttonLeftY1 + 5, "LEFT", WHITE, GBLUE, 16, 1);

    // Nút RIGHT
    lcd_Fill(buttonRightX1, buttonRightY1, buttonRightX2, buttonRightY2, GBLUE);
    lcd_ShowStr(buttonRightX1 + 10, buttonRightY1 + 5, "RIGHT", WHITE, GBLUE, 16, 1);
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
