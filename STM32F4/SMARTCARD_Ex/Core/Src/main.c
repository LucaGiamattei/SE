/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/** @brief Example of USART in Smartcard mode
 *
    Detailed description follows here.
    @author Gruppo 1: Giorgio Farina, Luca Giamattei, Gabriele Previtera
    @date April 2020
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
#define TRANSMITTER_BOARD
#define RESUMEDELAY 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SMARTCARD_HandleTypeDef hsc2;

/* USER CODE BEGIN PV */
/**
 * @var _IO ITStatus SmartcardReady
 *  ITStatus è un tipo enum che può assumere il valore RESET (zero unsigned) e SET (il suo negato)
 * _IO sta per volatile
 */

/**
 * @var _IO ITStatus SmartcardReady
 *  ITStatus è un tipo enum che può assumere il valore RESET (zero unsigned) e SET (il suo negato)
 * _IO sta per volatile
 * SmartcardReady è una variabile utilizzata per capire dal main se la ISR  (transmit e receive) è stata invocata
 */
__IO ITStatus SmartcardReady = RESET;


/**
 * @var _IO ITStatus ButtonPressed
 * ITStatus è un tipo enum che può assumere il valore RESET (zero unsigned) e SET (il suo negato)
 * _IO sta per volatile
 * ButtonPressed  è una variabile utilizzata per capire dal main se la ISR  (EXTI0) è stata invocata
 */
__IO ITStatus ButtonPressed = RESET;

/**
 * nReceived è una variabile utilizzata per tanere traccia del tipo di Ricezione
 * Nel caso del Transmitter Board:
 * nReceived = 1: ha ricevuto il riflesso del messaggio che ha inviato essendo anche il transmitter in modalità ricezione.
 * nReceived = 2: riceve il messaggio inviato dalla Receiver Board
 *
 * Nel caso del Receiver Board:
 * nReceived = 1: riceve il messaggio inviato dalla Transmitter Board
 * nReceived = 2: ha ricevuto il riflesso del messaggio che ha inviato essendo anche il Receiver in modalità ricezione.
 */
uint8_t nReceived = 0;

/** @var uint8_t aTxBuffer[]
 *  Buffer usato per la trasmissione di un intero: se è la board transmitter il buffer invierà il valore '1',
 * in codifica ASCII altrimenti se è la board receiver trasmetterà il valore '2' in codifica ASCII
 * Le costanti expected sono state usate per fare il controllo.
 */

#ifdef TRANSMITTER_BOARD
	uint8_t aTxBuffer[] = "1";
	uint8_t aRxExpected[] = "2";
#else
	uint8_t aRxExpected[] = "1";
	uint8_t aTxBuffer[] = "2";
#endif


/** @var uint8_t aRxBuffer[RXBUFFERSIZE]
	 * Buffer usato per la recezione del carattere */
	uint8_t aRxBuffer[RXBUFFERSIZE];


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_SMARTCARD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void){
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
  MX_USART2_SMARTCARD_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1){
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		#ifndef TRANSMITTER_BOARD
  		/**
  		 * Configura la Receiver Board per ricevere il primo carattere
		   */

	 	 	 if(HAL_SMARTCARD_Receive_IT(&hsc2, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK){
		 	   Error_Handler();
		 	 }

		#endif

	 	/**
	 	* Disattivazione della CPU, entrando nella modalità SleepMode
	 	* PWR_SLEEPENTRY_WFI: Risveglia la CPU quando si verifica un Interrupt
	 	*/

		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		/**
		 * Reset dei Led eccetto il LED 5 che indica la corretta ricezione del messaggio dell'altra board
		 */

		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED4_Pin, 0);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED6_Pin, 0);

		//Aspetto il risveglio
		HAL_Delay(RESUMEDELAY);

	#ifdef TRANSMITTER_BOARD
		/* Infinite Loop della TRANSMITTER BOARD */

		/** Tramite polling-interrupt il programma aspetta per la pressione del bottone USER prima di iniziare la comunicazione.
		 *	La ISR di EXTI0 aggiorna la variabile ButtonPressed
		 */
		while (ButtonPressed == RESET){

			/** Il Led 3 lampeggia finchè non viene premuto un bottone: il lampeggiamento del led indicherebbe che si è usciti
			 * dallo sleep mode non per aver premuto l'USER BUTTON
			 */

			HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
			HAL_Delay(40);
		}

		/** Reset del flag */
		ButtonPressed = RESET;

		/** Spegne il led 3 per indicare che inzio l'invio*/
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);

		/** Inizio del processo di trasmissione
		 * Trasmette il carattere contenuto nel buffer
		 */
		if(HAL_SMARTCARD_Transmit_IT(&hsc2, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK){
			Error_Handler();
		}

		/**
		 * Aspetta la fine della trasmissione (SmartcardReady è una variabile globale aggiornata nella callback dell'interruzione
		 */
		while (SmartcardReady != SET);

		/** Reset del flag SmartcardReady */
		SmartcardReady = RESET;

		/**
		 * Si prepara alla ricezione di due caratteri, il primo è quello che invia e il secondoe quello che gli invia l'altra board
		 * Imposta i parametri della ricezione, solo una volta conclusa la ricezione reimposterà i parametri.
		 */
		while(nReceived < 2){
			if(HAL_SMARTCARD_Receive_IT(&hsc2, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK){
				Error_Handler();
			}
		}

		/* Riaccensione del led3 per indicare che la board è la trasmittente */
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);

	#else
		/* Infinite Loop della RECEIVER BOARD */

		/* Attende la fine della ricezione */
	 	while (SmartcardReady != SET);

	 	/** Resetta il flag SmartcardReady*/
	 	SmartcardReady = RESET;

	 	/** La board receiver ha ricevuto il messaggio dal transmitter e provvederà a dare una risposta
	 	 * Inizia il processo di trasmissione
	 	 */
	 	if(HAL_SMARTCARD_Transmit_IT(&hsc2, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK){
	 		Error_Handler();
	 	}

	 	while (SmartcardReady != SET);

	 	SmartcardReady = RESET;

	 	if(HAL_SMARTCARD_Receive_IT(&hsc2, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK){
		 	   Error_Handler();
	 	}

	#endif

	 	/* Attendo il completamento delle operazioni di invio/ricezione sull'USART
	 	 * lo stato è cambiato dalle Callback
	 	 */
		while (SmartcardReady != SET);

	 	/* Azzero il contatore dei messaggi ricevuti */
		nReceived = 0;

		SmartcardReady = RESET;
  }
  /* USER CODE END 3 */
}
//End Main

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
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_SMARTCARD_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  hsc2.Instance = USART2;
  hsc2.Init.BaudRate = 115200;
  hsc2.Init.WordLength = SMARTCARD_WORDLENGTH_9B;
  hsc2.Init.StopBits = SMARTCARD_STOPBITS_1_5;
  hsc2.Init.Parity = SMARTCARD_PARITY_EVEN;
  hsc2.Init.Mode = SMARTCARD_MODE_TX_RX;
  hsc2.Init.GuardTime = 0;
  hsc2.Init.NACKState = SMARTCARD_NACK_DISABLE;
  if (HAL_SMARTCARD_Init(&hsc2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  else{
  	/* Accendo il led3 sulla transmitter board */
	  #ifdef TRANSMITTER_BOARD
	  	  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);
      #endif
  }

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED4_Pin|LED3_Pin|LED5_Pin|LED6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED4_Pin LED3_Pin LED5_Pin LED6_Pin */
  GPIO_InitStruct.Pin = LED4_Pin|LED3_Pin|LED5_Pin|LED6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */

void HAL_SMARTCARD_TxCpltCallback(SMARTCARD_HandleTypeDef * hsc)
{
	/**
	 * Set del flag SmartcardReady: transfer complete
	 */
  SmartcardReady = SET;

  /**
   *  LED 6 acceso: il trasferimento si è concluso
   */
  HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, 1);
}

void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef * hsc)
{
	/**
	 *  Set del flag SmartcardReady: receive complete
	 */
  SmartcardReady = SET;

  /**
   * Incremento della variabile nReceived
   */
  nReceived++;

  /**
   * LED 4 acceso: è stato ricevuto il carattere inviato dalla controparte.
   * Nel caso del transmitter board sta a significare: "La seconda ricezione consecutiva si è conclusa"
   * Nel caso del Receiver Board sta a significare: "La prima ricezione consecutiva si è conclusa"
   *
   */
	#ifdef TRANSMITTER_BOARD
  	if(nReceived == 2)
	#endif

  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 1);
  /**
    * Si contralla se il carattere ricevuto è quello che ci si aspettava di ricevere dalla controparte.
    * Nel caso in cui il controllo vada a buon fine, il LED 5 verrà acceso.
    */

  if(!Buffercmp(aRxBuffer,aRxExpected, RXBUFFERSIZE)){
 	 	 HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, 1);
  }

	#ifndef TRANSMITTER_BOARD
  	else if(nReceived !=2){
	  /**
	   * Nel Receiver board
	   * se il primo carattere ricevuto (quello della ricevuto dalla controparte) non è quello che ci si aspettava
	   * il led 5 viene spento
	   */
	  HAL_GPIO_WritePin(LED3_GPIO_Port, LED5_Pin, 0);
  }
	#endif
}

void HAL_SMARTCARD_ErrorCallback(SMARTCARD_HandleTypeDef * hsc){
 /* Tutti i led spendi: Transfer error in reception/transmission process */
	 HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
}


static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength){
  while (BufferLength){
    if ((*pBuffer1) != *pBuffer2){
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
    BufferLength--;
  }

  return 0;
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
