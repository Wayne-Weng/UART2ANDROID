/*
*		USART RS232 functions for HyperTerminal Commands.
*		|Author|	Wayne
*		|Date  |	2013.3.23
*		|Note  |	Configured with USART1(non-remapped).
*	
*/

#include "stm32f10x.h"
#include "USART_RS232.h"

//Command for new line & return carriage.
u8 newLineCmd[]={0x0A,0x0D,'\0'};	//why '\0' automatically set?, Just in case, adding '\0' tailing this cmd.

//string Buffer for USART_ReceiveString().
u8 strBuf[BUF_SIZE]={0};

//Command sets.
u8 cmd_Default[CMD_SIZE_Default]="Default";
u8 cmd_SetPA1[CMD_SIZE_SetPA1]="Set PA1";
u8 cmd_SetFreq[CMD_SIZE_SetFreq]="Set Freq";

void USART1_Init(void){
  
	//Create GPIO_InitStruct structure for all GPIO Config. usage.
	GPIO_InitTypeDef GPIO_InitStruct;
	//USART_InitStructure for USART Init config.
	USART_InitTypeDef USART_InitStructure;
	
	//Enable clock source for GPIOA, USART1 on APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);
	
	//GPIO Config. for PA9(USART1 TX) to AF PP.
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	
	//GPIO Init for PA9 Config.
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//GPIO Config. for PA10(USART1 RX) to Input Floating.
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	
	//GPIO Init for PA10 Config.
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
  //Config USART parameters.
  USART_InitStructure.USART_BaudRate = 115200;		//Fpclk2=72MHz will be more accurate.
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  //Init USART1 Peripheral.
  USART_Init(USART1,&USART_InitStructure);
  
  //Enable USART1 Peripheral.
  USART_Cmd(USART1,ENABLE);
	
	//Clear TC after Init(Or the fist data could be lost). Seems to be TC not RESET to 0 after Enabled. Gotta check.
	USART_ClearFlag(USART1,USART_FLAG_TC);
    
}

void USART_WriteByte(u8 word){
	
	//Send the 8bit word.
	USART_SendData(USART1,word);
	//Wait until TC(Transmission Complete flag) is SET(Completed).
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	//Clear TC instantly.
	USART_ClearFlag(USART1,USART_FLAG_TC);
	
}

u8 USART_ReceiveByte(void){
		
	//clear RXNE for the following reception.
// // 	USART_ClearFlag(USART1,USART_FLAG_RXNE);	
	
	//To in case of the pre-occured Overrun that wasn't cleared. This sequence will clear the ORE.
	//1.Make a read to SR reg.
	USART_GetFlagStatus(USART1,USART_FLAG_ORE);
	
	//2.followed by a read of DR. Also, it will automatically clear RXNE.
	USART_ReceiveData(USART1);
	
	//Wait until RXNE is SET(Data received in DR reg).
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)!=SET);
	
	//Get the received data and automatically clear RXNE.
	return USART_ReceiveData(USART1);
}

void USART_WriteString(u8 string[]){
	
	//variable for shift counter.(256 words limit).
	u8 s;
	//send the words until the end of line '\0'.(You can modify this function to meet 256 limit).
	for(s=0;string[s]!='\0';s++){
		USART_WriteByte(string[s]);
	}
	
}

void USART_ReceiveCmd(u8* strBuf,u8 strlen){
		
	//variable for shift counter.
	u8 s;
	
	//Temp byte to be each checked and verify Enter symbol.
	u8 byteTemp;
	
	//Verify the Enter symbol and place the end sign for the string. 
	for(s=0;s<strlen-1;s++){
		byteTemp=USART_ReceiveByte();
		if(byteTemp!=0x0D){
			USART_WriteByte(byteTemp);
			strBuf[s]=byteTemp;
		}
		else{
			strBuf[s]='\0';
			break;
		}
	}
	
	//set the final end sign of the string buffer.
	strBuf[sizeof(strBuf)-1]='\0';
	
}

void USART_ReceiveStdString(u8* strBuf,u8 strlen){
		
	//variable for shift counter.
	u8 s;
	
	//Temp byte to be each checked and verify Enter symbol.
	u8 byteTemp;
	
	//Collecting bytes.
	for(s=0;s<strlen-1;s++){
		byteTemp=USART_ReceiveByte();
		strBuf[s]=byteTemp;
		
	//Verify the NULL character and stop collecting bytes. 
		if(byteTemp=='\0') break;
	}
	
	//set the final end sign of the string buffer.
	strBuf[strlen-1]='\0';
	
}

FunctionalState USART_Cmd_Check(u8 cmdStr[],u8 sampleStr[],u8 checkLength){
	
	//variable for shift counter.
	u8 s;
	
	//status variable to be returned, it's initially set to ENABLE.
	FunctionalState status=ENABLE;
	
	//Check each char to see whether there is any mismatch or not. And if so, set the status DISABLE.
	for(s=0;s<checkLength;s++){
		if(sampleStr[s]!=cmdStr[s]){
			status=DISABLE;
		}
	}
	
	return status;
}

USART_Cmd_Type USART_Cmd_Switch(u8 sampleStr[]){
	
	//status variable to be returned, it's initially set to USART_CMD_Default.
	USART_Cmd_Type typeStatus=USART_CMD_Default;
	
	//check the cmd and get the type of cmd.
	if(USART_Cmd_Check(cmd_Default,sampleStr,CMD_SIZE_Default)==ENABLE){
		typeStatus=USART_CMD_Default;
	}
	else if(USART_Cmd_Check(cmd_SetPA1,sampleStr,CMD_SIZE_SetPA1)==ENABLE){
		typeStatus=USART_CMD_SetPA1;
	}
	else if(USART_Cmd_Check(cmd_SetFreq,sampleStr,CMD_SIZE_SetFreq)==ENABLE){
		typeStatus=USART_CMD_SetFreq;
	}
	else{
		typeStatus=USART_CMD_Invalid;
	}
	
	return typeStatus;
}

void USART_Cmd_Execute(u8 strBuf[],u8 strlen){
	
	//Collect a string data into strBuf.
	USART_ReceiveCmd(strBuf,strlen);
	
	//new line & carriage return.
	USART_WriteString(newLineCmd);
	
	//Get the USART_Cmd_Type.
	//Select the determined cmd and it's routine to execute.
	switch(USART_Cmd_Switch(strBuf)){
		case USART_CMD_SetPA1:
			USART_WriteString((u8*)"Cmd: Set PA1");
			break;
		case USART_CMD_SetFreq:
			USART_WriteString((u8*)"Cmd: Set Freq");
			break;
		case USART_CMD_Default:
			USART_WriteString((u8*)"Cmd: Default");
			break;
		case USART_CMD_Invalid:
			USART_WriteString((u8*)"Invalid Command!");
			break;
		default:
			break;
	}
	
	//new line & carriage return.
	USART_WriteString(newLineCmd);
	
}

static u16 RS232_VisualScope_CRC16( u8 *Array, u16 Len )
{
	u16 USART_IX, USART_IY, USART_CRC;

	USART_CRC = 0xffff;
	for(USART_IX=0; USART_IX<Len; USART_IX++) {
		USART_CRC = USART_CRC^(uint16_t)(Array[USART_IX]);
		for(USART_IY=0; USART_IY<=7; USART_IY++) {
			if((USART_CRC&1)!=0)
				USART_CRC = (USART_CRC>>1)^0xA001;
			else
				USART_CRC = USART_CRC>>1;
		}
	}
	return(USART_CRC);
}

void RS232_VisualScope( USART_TypeDef* USARTx, u8 *pWord, u16 Len )
{
	u8 i = 0;
	u16 Temp = 0;

	Temp = RS232_VisualScope_CRC16(pWord, Len);
	pWord[8] = Temp&0x00ff;
	pWord[9] = (Temp&0xff00)>>8;

	for(i=0; i<10; i++) {
		USART_SendData(USARTx, *pWord);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) != SET);
		pWord++;
	}
}
