/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306_fonts.h"
#include "ssd1306.h"

#include "stdio.h"
#include "stdbool.h"

#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define V25 		0.76
#define VSENSE		3.3 / 4096
#define AVG_SLOPE 	2.5

#define OUTPUT 0		/*!< OUTPUT: Output push pull at low frequency (2 MHz) */
#define INPUT 1			/*!< INPUT: Input with no pull resistor activation */
#define INPUT_PULLUP 2	/*!< INPUT_PULLUP: Input with pull up resistor activation */

#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_4

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
int8_t xIncrement = 0;
int8_t yIncrement = 0;

int8_t xIndex = 0;
int8_t yIndex = 0;

uint8_t manOn = 1;
uint8_t manOnIncrement = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len) {
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		ITM_SendChar(*ptr++);
	}
	return len;
}

void ADC_Select_CH0() {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

void ADC_Select_CH1() {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

void ADC_Select_CH2() {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

void ADC_Select_CH3() {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

void ADC_Select_CHTemp() {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

void delayMicroseconds(uint16_t Delay) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER(&htim1) < Delay)
		;
}

void pinMode(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t Mode) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = GPIO_Pin;

	if (Mode == OUTPUT) {
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	} else if (Mode == INPUT) {
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	} else if (Mode == INPUT_PULLUP) {
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}

	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT11_Init() {
	pinMode(DHT11_PORT, DHT11_PIN, OUTPUT);
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
	HAL_Delay(18);
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
	delayMicroseconds(20);
	pinMode(DHT11_PORT, DHT11_PIN, INPUT_PULLUP);
}

uint8_t DHT11_Handshake() {
	uint8_t Response = 0;
	delayMicroseconds(40);
	if ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) == GPIO_PIN_RESET) {
		delayMicroseconds(80);
		if ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) == GPIO_PIN_SET)
			Response = 1;
		else
			Response = 255;
	}
	delayMicroseconds(40);
//	while ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) == GPIO_PIN_SET)
//		;  // wait for the pin to go low
	return Response;
}

uint8_t DHT11_Read() {
	uint8_t i = 0;
	for (uint8_t j = 0; j < 8; j++) {
		while ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) == GPIO_PIN_RESET)
			;   // wait for the pin to go high`
		delayMicroseconds(40);   // wait for 40 us
		if ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) == GPIO_PIN_RESET) // if the pin is low
				{
			i &= ~(1 << (7 - j));   // write 0
		} else
			i |= (1 << (7 - j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) == GPIO_PIN_SET)
			;  // wait for the pin to go low
	}
	return i;
}

void reverse(char str[], int length) {
	int start = 0;
	int end = length - 1;
	while (start < end) {
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		end--;
		start++;
	}
}
// Implementation of citoa()
char* citoa(int num, char *str, int base) {
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitly, otherwise empty string is
	 * printed for 0 */
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled
	// only with base 10. Otherwise numbers are
	// considered unsigned.
	if (num < 0 && base == 10) {
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while (num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// If number is negative, append '-'
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator

	// Reverse the string
	reverse(str, i);

	return str;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	MX_ADC1_Init();
	MX_TIM1_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

	char row0[10][9][32] = { { "Real-Time Data", "Forecast data", "Debug",
			"Manual Override", "\0" }, { "Soil Moisture", "Temperature",
			"Humidity", "Rain", "Light", "\0" }, { "Temperature", "Humidity",
			"Precipitation", "Cloud Cover", "Soil Moisture", "\0" }, {
			"Core Temp", "Core Voltage", "Sync Data", "\0" }, { "Temperature",
			"Temperature", "Temperature", "Temperature", "Temperature",
			"Temperature", "Temperature", "Temperature", "\0" }, { "Humidity",
			"Humidity", "Humidity", "Humidity", "Humidity", "Humidity",
			"Humidity", "Humidity", "\0" }, { "Precipitation", "Precipitation",
			"Precipitation", "Precipitation", "Precipitation", "Precipitation",
			"Precipitation", "Precipitation", "\0" }, { "Cloud Cover",
			"Cloud Cover", "Cloud Cover", "Cloud Cover", "Cloud Cover",
			"Cloud Cover", "Cloud Cover", "Cloud Cover", "\0" }, {
			"Soil Moisture", "Soil Moisture", "Soil Moisture", "Soil Moisture",
			"Soil Moisture", "Soil Moisture", "Soil Moisture", "Soil Moisture",
			"\0" }, { "\0" } };

	char row1[10][9][16] = { { "", "", "", "%d" }, { "%d", "%dC", "%d%%", "%d%",
			"%d", "\0" }, { "\0" }, { "%0.1fC", "%0.1fV", "\0" }, { "%0.1fC",
			"%0.1fC", "%0.1fC", "%0.1fC", "%0.1fC", "%0.1fC", "%0.1fC",
			"%0.1fC", "\0" }, { "%d%%", "%d%%", "%d%%", "%d%%", "%d%%", "%d%%",
			"%d%%", "%d%%", "\0" }, { "%dmm(%d%%)", "%dmm(%d%%)", "%dmm(%d%%)",
			"%dmm(%d%%)", "%dmm(%d%%)", "%dmm(%d%%)", "%dmm(%d%%)",
			"%dmm(%d%%)", "\0" }, { "%d%%", "%d%%", "%d%%", "%d%%", "%d%%",
			"%d%%", "%d%%", "%d%%", "\0" }, { "%0.3f", "%0.3f", "%0.3f", "%0.3f", "%0.3f",
			"%0.3f", "%0.3f", "%0.3f", "\0" }, { "\0" } };

	uint16_t varInt[10][9];
	float varFloat[10][9];

	for (uint8_t i = 0; i < 10; i++) {
		for (uint8_t j = 0; j < 9; j++) {
			varFloat[i][j] = 0;
			varInt[i][j] = 0;
		}
	}

	ssd1306_Init();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("Syncing", Font_11x18, White);
	ssd1306_SetCursor(0, 20);
	ssd1306_WriteString("Data...", Font_11x18, White);
	ssd1306_UpdateScreen();

	char ATcommand[256];
	char ATmessage[256];

	uint8_t ATisOK = 0;
	uint8_t rxBuffer[8192] = { 0 };

	sprintf(ATcommand, "AT+RST\r\n");
	memset(rxBuffer, 0, sizeof(rxBuffer));
	HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand), 1000);
	HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
	printf("ESP01 reset\n\r");
	HAL_Delay(500);

	printf("Enabling STA...\r");
	ATisOK = 0;
	while (!ATisOK) {
		sprintf(ATcommand, "AT+CWMODE_CUR=1\r\n");
		memset(rxBuffer, 0, sizeof(rxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand),
				1000);
		HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
		if (strstr((char*) rxBuffer, "OK")) {
			ATisOK = 1;
			printf("Enabled\n\r");
		}
		HAL_Delay(500);
	}

	printf("Connecting to WiFi...\r");
	ATisOK = 0;
	while (!ATisOK) {
		sprintf(ATcommand, "AT+CWJAP_CUR=\"moto x4 6475\",\"IRONMANROX\"\r\n");
		memset(rxBuffer, 0, sizeof(rxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand),
				1000);
		HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 20000);
		if (strstr((char*) rxBuffer, "OK")) {
			ATisOK = 1;
			printf("Connected\n\r");
		}
		HAL_Delay(500);
	}

	printf("Enabling single connection mode...\r");
	ATisOK = 0;
	while (!ATisOK) {
		sprintf(ATcommand, "AT+CIPMUX=0\r\n");
		memset(rxBuffer, 0, sizeof(rxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand),
				1000);
		HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
		if (strstr((char*) rxBuffer, "OK")) {
			ATisOK = 1;
			printf("Enabled\n\r");
		}
		HAL_Delay(500);
	}

	printf("Connecting to Open-Meteo...\r");
	sprintf(ATcommand, "AT+CIPSTART=\"TCP\",\"api.open-meteo.com\",80\r\n");
	memset(rxBuffer, 0, sizeof(rxBuffer));
	HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand), 1000);
	HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
	printf("Connected\n\r");
	HAL_Delay(500);

	printf("Fetching data...\r");
	sprintf(ATmessage,
			"GET /v1/forecast?latitude=19.9973&longitude=73.791&hourly=temperature_2m&forecast_days=2 HTTP/1.1\r\n"
					"Host: api.open-meteo.com\r\n"
					"Connection: close\r\n\r\n");
	sprintf(ATcommand, "AT+CIPSEND=%d\r\n", strlen(ATmessage));
	memset(rxBuffer, 0, sizeof(rxBuffer));
	HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand), 1000);
	HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
	if (strstr((char*) rxBuffer, ">")) {
		memset(rxBuffer, 0, sizeof(rxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t*) ATmessage, strlen(ATmessage),
				1000);
		HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
	}

	char parsedTime[6] = "";
	for (uint8_t i = 0; i < 5; i++) {
		parsedTime[i] = rxBuffer[i + 81];
	}
	parsedTime[5] = '\0';

	uint8_t parsedHrs = (parsedTime[0] - '0') * 10 + (parsedTime[1] - '0');
	uint8_t parsedMins = (parsedTime[3] - '0') * 10 + (parsedTime[4] - '0');
	printf("Parsed time: %d:%d\n", parsedHrs, parsedMins);

	char parsedString[56] = "";
	uint8_t commaCount = 0;
	uint8_t i = 0;
	uint16_t dispHrs = parsedMins < 30 ? (parsedHrs + 6) : (parsedHrs + 7);
	dispHrs = dispHrs > 24 ? dispHrs - 24 : dispHrs;
	uint8_t startIndex =
			parsedMins < 30 ? (parsedHrs + 1) * 5 : (parsedHrs + 2) * 5;
	float parsedTemps[8];
	while (commaCount < 8) {
		parsedString[i] = rxBuffer[i + startIndex + 1376];
		if (rxBuffer[i + 1376] == ',') {
			parsedTemps[commaCount] = parsedString[i - 1] * 0.1
					+ parsedString[i - 3] + parsedString[i - 4] * 10 - 532.8;
			commaCount++;
		}
		i++;
	}
	parsedString[i - 1] = '\0'; // Null-terminate parsedString
//	printf("%s\n", parsedString);

	for (uint8_t i = 0; i < 8; i++) {
		varFloat[4][i] = parsedTemps[i];
	}

	for (i = 0; i < 8; i++) {
		printf(
				dispHrs + i == 0 ?
						"000%d: " : (dispHrs + i < 10 ? "0%d00: " : "%d00: "),
				dispHrs + i);
		printf("%0.1f\n", varFloat[4][i]);
	}

	//Hums
	printf("Connecting to Open-Meteo...\r");
	sprintf(ATcommand, "AT+CIPSTART=\"TCP\",\"api.open-meteo.com\",80\r\n");
	memset(rxBuffer, 0, sizeof(rxBuffer));
	HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand), 1000);
	HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
	printf("Connected\n\r");
	HAL_Delay(500);

	printf("Fetching data...\r");
	sprintf(ATmessage,
			"GET /v1/forecast?latitude=19.9973&longitude=73.791&hourly=relative_humidity_2m&forecast_days=2 HTTP/1.1\r\n"
					"Host: api.open-meteo.com\r\n"
					"Connection: close\r\n\r\n");
	sprintf(ATcommand, "AT+CIPSEND=%d\r\n", strlen(ATmessage));
	memset(rxBuffer, 0, sizeof(rxBuffer));
	HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand), 1000);
	HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
	if (strstr((char*) rxBuffer, ">")) {
		memset(rxBuffer, 0, sizeof(rxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t*) ATmessage, strlen(ATmessage),
				1000);
		HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
	}

	memset(parsedString, 0, sizeof(parsedString));
	commaCount = 0;
	i = 0;
	startIndex = parsedMins < 30 ? (parsedHrs + 1) * 3 : (parsedHrs + 2) * 3;
	uint16_t parsedHums[8];
	while (commaCount < 8) {
		parsedString[i] = rxBuffer[i + startIndex + 1385];
		if (rxBuffer[i + 1385] == ',') {
			parsedHums[commaCount] = parsedString[i - 1]
					+ parsedString[i - 2] * 10 - 528;
			commaCount++;
		}
		i++;
	}
	parsedString[i - 1] = '\0'; // Null-terminate parsedString
//	printf("%s\n", parsedString);

	for (uint8_t i = 0; i < 8; i++) {
		varInt[5][i] = parsedHums[i];
	}

	//Clouds
	sprintf(ATcommand, "AT+CIPSTART=\"TCP\",\"api.open-meteo.com\",80\r\n");
	memset(rxBuffer, 0, sizeof(rxBuffer));
	HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand), 1000);
	HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
//	printf("Connected\n\r");
	HAL_Delay(500);

	printf("Fetching data...\r");
	sprintf(ATmessage,
			"GET /v1/forecast?latitude=19.9973&longitude=73.791&hourly=cloud_cover,soil_moisture_0_to_1cm&forecast_days=2 HTTP/1.1\r\n"
					"Host: api.open-meteo.com\r\n"
					"Connection: close\r\n\r\n");
	sprintf(ATcommand, "AT+CIPSEND=%d\r\n", strlen(ATmessage));
	memset(rxBuffer, 0, sizeof(rxBuffer));
	HAL_UART_Transmit(&huart1, (uint8_t*) ATcommand, strlen(ATcommand), 1000);
	HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
	if (strstr((char*) rxBuffer, ">")) {
		memset(rxBuffer, 0, sizeof(rxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t*) ATmessage, strlen(ATmessage), 1000);
		HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 1000);
//		printf("%s\n\r", rxBuffer);
	}

	memset(parsedString, 0, sizeof(parsedString));
	commaCount = 0;
	i = 0;
	startIndex = 0;
	uint8_t parsedClouds[8];
	while (commaCount < parsedHrs + 1) {
		if (rxBuffer[startIndex + 1402] == ',') {
			commaCount++;
		}
		startIndex++;
	}

	commaCount = 0;
	while (commaCount < 8) {
		parsedString[i] = rxBuffer[i + startIndex + 1402];
		if (rxBuffer[i + startIndex + 1402] == ',') {
			if (parsedString[i - 2] == ',') {
				parsedClouds[commaCount] = parsedString[i - 1] - 48;
			} else {
				parsedClouds[commaCount] = parsedString[i - 1]
						+ parsedString[i - 2] * 10 - 528;
			}
			commaCount++;
		}
		i++;
	}
	parsedString[i - 1] = '\0'; // Null-terminate parsedString
	printf("%s\n", parsedString);

	for (uint8_t i = 0; i < 8; i++) {
		varInt[7][i] = parsedClouds[i];
	}

	for (i = 0; i < 8; i++) {
		printf(
				dispHrs + i == 0 ?
						"000%d: " : (dispHrs + i < 10 ? "0%d00: " : "%d00: "),
				dispHrs + i);
		printf("%d\n", varInt[5][i]);
	}

	memset(parsedString, 0, sizeof(parsedString));
	commaCount = 0;
	i = 0;
	startIndex = 0;
	float parsedSoils[8];
	while (commaCount < parsedHrs + 1) {
		if (rxBuffer[startIndex + 1546] == ',') {
			commaCount++;
		}
		startIndex++;
	}

	commaCount = 0;
	while (commaCount < 8) {
		parsedString[i] = rxBuffer[i + startIndex + 1546];
		if (rxBuffer[i + startIndex + 1546] == ',') {
			parsedSoils[commaCount] = parsedString[i - 1] * 0.001 +
						+ parsedString[i - 2] * 0.01 + parsedString[i - 3] * 0.1 - 5.328;
			commaCount++;
		}
		i++;
	}
	parsedString[i - 1] = '\0'; // Null-terminate parsedString
	printf("%s\n", parsedString);

	for (uint8_t i = 0; i < 8; i++) {
		varFloat[8][i] = parsedSoils[i];
	}

	printf("armdude done. Thank you.\n");
	HAL_Delay(500);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		char message[32];

		if (xIncrement != 0) {
			xIndex += xIncrement;
			xIncrement = 0;

			if (xIndex < 0) {
				xIndex = 0;
			} else if (row0[yIndex][xIndex][0] == '\0') {
				xIndex--;
			}

			HAL_Delay(50);
		}

		if (yIncrement != 0) {
			yIndex += yIncrement;
			yIncrement = 0;
			xIndex = 0;

			if (yIndex < 0) {
				yIndex = 0;
			}

			HAL_Delay(50);
		}

		if (manOnIncrement != 0) {
			manOn += manOnIncrement;
			manOnIncrement = 0;

			if (manOn < 0) {
				manOn = 0;
			} else if (manOn > 2) {
				manOn = 2;
			}

			HAL_Delay(50);
		}

		ADC_Select_CHTemp(); //Core Temp
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		varFloat[3][0] = ((V25 - HAL_ADC_GetValue(&hadc1) * VSENSE) / AVG_SLOPE)
				+ 25;
		HAL_ADC_Stop(&hadc1);

		ADC_Select_CH0(); //Core Voltage
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		varFloat[3][1] = HAL_ADC_GetValue(&hadc1) * VSENSE * 2;
		HAL_ADC_Stop(&hadc1);

		ADC_Select_CH1(); //SMS
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		varInt[1][0] = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		ADC_Select_CH2(); //Rain
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		varInt[1][3] = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		ADC_Select_CH3(); //Light
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		varInt[1][4] = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		DHT11_Init();
		DHT11_Handshake();
		varInt[1][2] = DHT11_Read();
		DHT11_Read();
		varInt[1][1] = DHT11_Read();
		DHT11_Read();
		DHT11_Read();

		ssd1306_Fill(Black);
		ssd1306_SetCursor(0, 0);
		sprintf(message, "%s", row0[yIndex][xIndex]);
		ssd1306_WriteString(message, Font_7x10, White);

		ssd1306_SetCursor(0, 20);
		if (yIndex == 0 && xIndex == 3) {
			if (manOn == 0) {
				ssd1306_WriteString("OFF", Font_11x18, White);
			} else if (manOn == 1){
				ssd1306_WriteString("AUTO", Font_11x18, White);
			} else if (manOn == 2){
				ssd1306_WriteString("ON", Font_11x18, White);
			}
		} else {
			if (varFloat[yIndex][xIndex] != 0) {
				sprintf(message, row1[yIndex][xIndex], varFloat[yIndex][xIndex],
						varFloat[yIndex][xIndex]);
			} else {
				sprintf(message, row1[yIndex][xIndex], varInt[yIndex][xIndex],
						varInt[yIndex][xIndex]);
			}
			ssd1306_WriteString(message, Font_11x18, White);
		}

		if (yIndex > 3) {
			ssd1306_SetCursor(0, 50);
			sprintf(message,
					dispHrs + xIndex == 0 ?
							"000%d" :
							(dispHrs + xIndex < 10 ? "0%d00" : "%d00"),
					dispHrs + xIndex);
			ssd1306_WriteString(message, Font_7x10, White);
		}

		ssd1306_UpdateScreen();

		if (varInt[1][0] >= 4000 && varInt[1][3] >= 3000 && varInt[1][1] >= 21) {
//			if (varInt[1][0] >= 1023
//					|| (varFloat[1][1] >= 31.1 && varFloat[1][2] <= 32.2)
//					|| (varFloat[1][2] >= 55 && varFloat[1][2] <= 60)) {
			varInt[0][3] = 65535;
//			} else {
//				varInt[0][3] = 0;
//			}
		} else {
			varInt[0][3] = 0;
		}

		if (manOn == 0) {
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		} else if (manOn == 1) {
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, varInt[0][3]);
		} else if (manOn == 2) {
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 65535);
		}

//		printf("%d", varInt[0][3]);
		HAL_Delay(1000);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

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
	RCC_OscInitStruct.PLL.PLLN = 96;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ENABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 5;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = 3;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = 4;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = 5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 400000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 95;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 65535;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */
	HAL_TIM_MspPostInit(&htim1);

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PB12 PB13 PB14 PB15 */
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_12) {
		xIncrement = -1;
	} else if (GPIO_Pin == GPIO_PIN_13) {
		if (yIndex == 0 && xIndex != 3) {
			yIncrement = xIndex + 1;
		} else if (yIndex == 2) {
			yIncrement = xIndex + 2;
		} else if (yIndex == 0 && xIndex == 3) {
			manOnIncrement = 1;
		}
	} else if (GPIO_Pin == GPIO_PIN_14) {
		xIncrement = 1;
	} else if (GPIO_Pin == GPIO_PIN_15) {
		if (yIndex >= 1 && yIndex <= 3) {
			yIncrement = -yIndex;
		} else if (yIndex >= 4) {
			yIncrement = 2 - yIndex;
		} else if (yIndex == 0 && xIndex == 3) {
			manOnIncrement = -1;
		}

	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
