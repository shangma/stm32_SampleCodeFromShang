/***
Minimum project from Shang Ma as a template for future use
***/

#include "main.h"
#include <stdio.h>


#define GPIO_PIN_X GPIO_Pin_6
#define GPIO_PIN_Y GPIO_Pin_7
#define BSRR_VAL 0x00C0
#define BSRR_VAL6 0x0040
#define BSRR_VAL7 0x0080

#define ADC1_DR_ADDRESS    ((uint32_t)0x40012458)

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

__IO uint16_t ADC_ConvertedValue;

static __IO uint32_t TimingDelay;
	
void Delay(__IO uint32_t nCount);
void LED_Init(void);
void USART3_Init(void);
void USART3Put(uint8_t ch);
uint8_t USART3Get(void);

void ADC_DMA_Config(void);
	
void EXTI0_Config(void);


int main(void)
{
		LED_Init();
		USART3_Init();
		EXTI0_Config();

	  printf("\r\n%s, %d\r\n", "ARM stm", 32);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
		{}
		

			
		while (1)
		{
				GPIOB->BSRRL = BSRR_VAL7;
				Delay(1000000);
				GPIOB->BSRRH = BSRR_VAL7;
				Delay(1000000);
		}
}

void LED_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
		/* Configure PD0 and PD1 or PD3 and PD7 in output pushpull mode */
		GPIO_InitStructure.GPIO_Pin = GPIO_PIN_X | GPIO_PIN_Y;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void USART3_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
		USART_InitStructure.USART_BaudRate = 9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		/* Enable GPIO clock */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	  /* Enable USART3 clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	  /* Connect PXx to USARTx_Tx */
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);

		/* Connect PXx to USARTx_Rx */
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
		
		/* Configure USART Tx as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		/* Configure USART Rx as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		/* USART configuration */
		USART_Init(USART3, &USART_InitStructure);
    
		/* Enable USART */
		USART_Cmd(USART3, ENABLE);
}

void ADC_DMA_Config(void)
{
		ADC_InitTypeDef ADC_InitStructure;
		DMA_InitTypeDef DMA_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;

		
		/* Enable DMA1 clock */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		/* DMA1 channel1 configuration */
		DMA_DeInit(DMA1_Channel1);
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = 1;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel1, &DMA_InitStructure);
		
		/* Enable DMA1 channel1 */
		DMA_Cmd(DMA1_Channel1, ENABLE);

		/*----------------- ADC1 configuration with DMA enabled --------------------*/
		/* Enable the HSI oscillator */
		//RCC_HSICmd(ENABLE);

		/* Enable GPIOB clock */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		/* Configure PB.12 (ADC Channel18) in analog mode */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStructure);


		/* Check that HSI oscillator is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

		/* Enable ADC1 clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		/* ADC1 configuration */
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfConversion = 1;
		ADC_Init(ADC1, &ADC_InitStructure);

		/* ADC1 regular channel18 configuration */ 
		ADC_RegularChannelConfig(ADC1, ADC_Channel_18, 1, ADC_SampleTime_4Cycles);


		/* Enable the request after last transfer for DMA Circular mode */
		ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
		
		/* Enable ADC1 DMA */
		ADC_DMACmd(ADC1, ENABLE);
		
		/* Enable ADC1 */
		ADC_Cmd(ADC1, ENABLE);

		/* Wait until the ADC1 is ready */
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
		{
		}

		/* Start ADC1 Software Conversion */ 
		ADC_SoftwareStartConv(ADC1);

}

void EXTI0_Config(void)
{
		EXTI_InitTypeDef   EXTI_InitStructure;
		GPIO_InitTypeDef   GPIO_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
	  
		/* Enable GPIOA clock */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		/* Configure PA0 pin as input floating */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	  /* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		/* Connect EXTI0 Line to PA0 pin */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	  /* Configure EXTI0 line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		/* Enable and set EXTI0 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}

void USART3Put(uint8_t ch)
{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
    {
    }
		USART_SendData(USART3, (uint8_t) ch);
}

uint8_t USART3Get(void)
{
		while ( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
					
		return (uint8_t)USART_ReceiveData(USART3);
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART3, (uint8_t) ch);

  /* Loop until transmit data register is empty */
  while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
  {}

  return ch;
}

void ToggleLed(void)
{
		GPIOB->BSRRL = BSRR_VAL6;
		Delay(1000000);
		GPIOB->BSRRH = BSRR_VAL6;
		Delay(1000000);
}
/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
		if (TimingDelay != 0x00)
		{ 
			TimingDelay--;
		}
}

/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
		while(nCount--)
		{
		}
}
