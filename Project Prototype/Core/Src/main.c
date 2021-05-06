/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t hexToAscii(uint8_t n) //4-bit hex value converted to an ascii character
{
    if (n >= 0 && n <= 9)
        n = n + '0';
    else
        n = n - 10 + 'A';
    return n;
}
float hextoint(uint8_t *in)
{
    float out = 0.0;
    if (in[0] >= 48 && in[0] <= 57)
        out += ((in[0] - '0') * 16);
    else
        out += ((in[0] - 'A' + 10) * 16);
    if (in[1] >= 48 && in[1] <= 57)
        out += (in[1] - '0');
    else
        out += (in[1] - 'A' + 10);
    if (in[3] >= 48 && in[3] <= 57)
        out += ((in[3] - '0') / 16.0);
    else
        out += ((in[3] - 'A' + 10) / 16.0);
    return out;
}

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
    MX_I2C1_Init();
    MX_USART2_UART_Init();
    uint8_t secbuffer[2], minbuffer[2], hourbuffer[2];
    // seconds
    secbuffer[0] = 0x00; //register address
    secbuffer[1] = 0x00; //data to put in register --> 0 sec
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, secbuffer, 2, 10);
    // minutes
    minbuffer[0] = 0x01; //register address
    minbuffer[1] = 0x15; //data to put in register --> 15 min
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, minbuffer, 2, 10);
    // hours
    hourbuffer[0] = 0x02; //register address
    hourbuffer[1] = 0x47; //data to put in register 01001001 --> 7 am
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, hourbuffer, 2, 10);
    //Receive via I2C and forward to UART
    char out[] = {0, 0, ':', 0, 0, ':', 0, 0, '\0'};
    float temp;
    uint8_t tempLSB[2], tempMSB[2];
    uint8_t eosc[2];
    uint8_t osf[2];
    char out_temp[50];
    float current;
    int steps_displayed;
    char steps_string[50];
    uint8_t tempbuffer1[2], tempbuffer2[2];
    tempbuffer1[0] = 0x11;
    tempbuffer2[0] = 0x12;
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempbuffer1, 2, 10);
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempbuffer2, 2, 10);
    /* USER CODE END 2 */
    //uint8_t out_temp[] = {0,0,'.',0,0, '\0'};
    while (1)
    {
        //send seconds register address 00h to read from
        HAL_I2C_Master_Transmit(&hi2c1, 0xD0, secbuffer, 1, 10);
        //read data of register 00h to secbuffer[1]
        HAL_I2C_Master_Receive(&hi2c1, 0xD1, secbuffer + 1, 1, 10);
        //prepare UART output
        out[6] = hexToAscii(secbuffer[1] >> 4);
        out[7] = hexToAscii(secbuffer[1] & 0x0F);
        HAL_I2C_Master_Transmit(&hi2c1, 0xD0, minbuffer, 1, 10);
        HAL_I2C_Master_Receive(&hi2c1, 0xD1, minbuffer + 1, 1, 10);
        out[3] = hexToAscii(minbuffer[1] >> 4);
        out[4] = hexToAscii(minbuffer[1] & 0x0F);
        HAL_I2C_Master_Transmit(&hi2c1, 0xD0, hourbuffer, 1, 10);
        HAL_I2C_Master_Receive(&hi2c1, 0xD1, hourbuffer + 1, 1, 10);
        out[0] = hexToAscii((hourbuffer[1] >> 4) & 1);
        out[1] = hexToAscii(hourbuffer[1] & 0x0F);
        // transmit time to UART

        tempMSB[0] = 0x11;
        tempLSB[0] = 0x12;

        eosc[0] = 0x0E;
        eosc[1] = 0x3C;
        HAL_I2C_Master_Transmit(&hi2c1, 0xD0, eosc, 2, 10);

        osf[0] = 0x0E;
        osf[1] = 0x3C;
        HAL_I2C_Master_Transmit(&hi2c1, 0xD0, osf, 2, 10);

        //send to register address 11h
        HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempMSB, 1, 10);
        //read data of register 11h to tempMSP[1]
        HAL_I2C_Master_Receive(&hi2c1, 0xD1, tempMSB + 1, 1, 10);
        temp = tempMSB[1];

        //send to register address 11h
        HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempLSB, 1, 10);
        //read data of register 11h to tempMSP[1]
        HAL_I2C_Master_Receive(&hi2c1, 0xD1, tempLSB + 1, 1, 10);
        //actual_temp_decimal=tempLSB[1];
        tempLSB[1] = tempLSB[1] >> 6;
        temp = temp + tempLSB[1] * 0.25;

        sprintf(out_temp, "%.2f deg.C", temp);

        //	//send temp register address 11h to read from
        //	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempbuffer1, 1, 10);
        //	//read data of register 11h to tempbuffer1[1]
        //	HAL_I2C_Master_Receive(&hi2c1, 0xD1, tempbuffer1+1, 1, 10);
        //	//send temp register address 12h to read from
        //	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempbuffer2, 1, 10);
        //	//read data of register 12h to tempbuffer2[1]
        //	HAL_I2C_Master_Receive(&hi2c1, 0xD1, tempbuffer2+1, 1, 10);
        //	//out
        ////	out_temp[3] = hexToAscii(tempbuffer2[1] >> 4 );
        ////	out_temp[4] = hexToAscii(tempbuffer2[1] & 0x0F );
        //	if(tempbuffer2[1]>>6==0){
        //		out_temp[3] = '0';
        //		out_temp[4] = '0';
        //	}
        //	if(tempbuffer2[1]>>6==1){
        //		out_temp[3] = '2';
        //		out_temp[4] = '5';
        //	}
        //	if(tempbuffer2[1]>>6==2){
        //		out_temp[3] = '5';
        //		out_temp[4] = '0';
        //	}
        //	if(tempbuffer2[1]>>6==3){
        //		out_temp[3] = '7';
        //		out_temp[4] = '5';
        //	}
        HAL_Delay(100);
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
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Configure LSE Drive Capability
  */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_I2C1;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure the main internal regulator output voltage
  */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }
    /** Enable MSI Auto calibration
  */
    HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x00000004;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure Analogue filter
  */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure Digital filter
  */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

    /*Configure GPIO pin : PA5 */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : PA6 */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

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

#ifdef USE_FULL_ASSERT
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
