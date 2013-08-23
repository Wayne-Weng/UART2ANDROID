/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f1_system.h"
#include "stm32f1_usart.h"
#include "USART_RS232.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
#define LED_R   PCO(13)
#define LED_G   PCO(14)
#define LED_B   PCO(15)

#define KEY_WU  PAI(0)
#define KEY_BO  PBI(2)

#define OUT1  PAO(1)
#define OUT2  PAO(2)
/*=====================================================================================================*/
/*=====================================================================================================*/
void GPIO_Config( void );
/*=====================================================================================================*/
/*=====================================================================================================*/
u8 str1[10] = "relay1";
u8 str2[10] = "relay2";
/*=====================================================================================================*/
//STR_CMP(array1,array2,10)
/*=====================================================================================================*/
u8 STR_CMP(u8 *str1,u8 *str2,u8 len)
{
	u8 s=0;
	
	for(s=0;s<len;s++){
		if(str1[s]!=str2[s])
			return 0;
  }
	
	return 1;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
int main( void )
{
  u8 TrData[10] = {0};
  u8 RrData[7] = {0};

	GPIO_Config();

	USART1_Init();

  while(1) {
		
		USART_WriteString((u8*)"Waiting for cmommand!\r\n");
    
    USART_ReceiveStdString(RrData,sizeof(RrData));
	  
		USART_WriteString((u8*)"USART Received: ");
		USART_WriteString(RrData);
		USART_WriteString((u8*)"\n\r");
		
    if(STR_CMP(RrData,str1,6)){
			USART_WriteString((u8*)"Relay1 toggle\n\r");
			OUT1 = ~OUT1;
			LED_B = ~LED_B;
    }else if(STR_CMP(RrData,str2,6)){
			USART_WriteString((u8*)"Relay2 toggle\n\r");
			OUT2 = ~OUT2;
			LED_R = ~LED_R;
    }		
		
		LED_G = ~LED_G;
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void GPIO_Config( void )
{
  GPIO_InitTypeDef GPIO_InitStruct;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

  /* PC13 LED_B */	/* PC14 LED_G */	/* PC15 LED_R */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* KEY_WU PA0 */	/* KEY_BO PB2 */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	/* OUT1 PA1 */  /* OUT2 PA2 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
	OUT1 = 0;
	OUT2 = 0;
	
  LED_R = 1;
  LED_G = 1;
  LED_B = 1;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
