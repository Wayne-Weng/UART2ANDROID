/*
*		USART RS232 functions for HyperTerminal Commands.
*		|Author|	Wayne
*		|Date  |	2013.3.23
*		|Note  |	Functions from USART_RS232.c file.
*
*/

#include "stm32f10x.h"

#ifndef _USART_RS232_
#define	_USART_RS232_

	#define BUF_SIZE 50
	
	#define CMD_SIZE_Default 8
	#define CMD_SIZE_SetPA1 8
	#define CMD_SIZE_SetFreq 9
	
	typedef enum {
		USART_CMD_SetPA1 = 0, 
		USART_CMD_SetFreq = 1,
		USART_CMD_Default = 10,
		USART_CMD_Invalid = 11
	} USART_Cmd_Type;
	
	extern u8 newLineCmd[];
	extern u8 strBuf[BUF_SIZE];
	
	extern u8 cmd_Default[CMD_SIZE_Default];
	extern u8 cmd_SetPA1[CMD_SIZE_SetPA1];
	extern u8 cmd_SetFreq[CMD_SIZE_SetFreq];
	
	void USART1_Init(void);
	void USART_WriteByte(u8 word);
	u8 USART_ReceiveByte(void);
	void USART_WriteString(u8 string[]);
	void USART_ReceiveCmd(u8* strBuf,u8 strlen);
	void USART_ReceiveStdString(u8* strBuf,u8 strlen);
	FunctionalState USART_Cmd_Check(u8 cmdStr[],u8 sampleStr[],u8 checkLength);
	USART_Cmd_Type USART_Cmd_Switch(u8 sampleStr[]);
	void USART_Cmd_Execute(u8 strBuf[],u8 strlen);
	static u16 RS232_VisualScope_CRC16( u8 *Array, u16 Len );
	void RS232_VisualScope( USART_TypeDef* USARTx, u8 *pWord, u16 Len );
	
#endif
