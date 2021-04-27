#include "SIM868.h"
#include "Usart3.h"
#include "delay.h"
#define GPRSR_MAX  256
 
typedef unsigned char* UCHARP;

 //uint8_t GPRS_Rbuffer[GPRSR_MAX];
 
char *Defualt_URL= "AT+CIPSTART=\"TCP\",\"mqtt-2.omnivoltaic.com\",\"1883\"\r\n";


 void SIM800_Wait(u16 delay)
 {
	 delay_ms(delay); 
		 
 }


u16 Omnivoltaic_GPRSCommand(char *cmd, char *answer, u16 wait_time)
{
  char num = 0;
  char *command;
	u16 len=0;	
	u8 FindOK=0;	
  command = cmd;

  do
  {
			Uart3.ClearRxBuff(); //每次重发先清空jieshou缓冲区
			len=strlen(command) ;
		  Uart3.SendData((u8*)command,len,1000);

      //查找回复字段 如果没有得正确回复继续执行	
			if( !FindOK ){ FindOK=FindStrFormSim800cResponds(answer,1,wait_time);	}
			if( !FindOK ){ FindOK=FindStrFormSim800cResponds(answer,1,wait_time);	}
			if( !FindOK ){ FindOK=FindStrFormSim800cResponds(answer,1,wait_time);	}
			if( !FindOK ){ FindOK=FindStrFormSim800cResponds(answer,1,wait_time);	}
			
  }while(0);//(FindAnswer == 0);
	
	return 1;
}

void USART_DEBUG_SendStr(char * str)
{
	
	
}

u16 Omnivoltaic_GPRSConnctTcp(char* ipaddr  ,int port)
{
	//*
  //USART_DEBUG_SendStr("START SIM800C \r\n");
  Omnivoltaic_GPRSCommand("ATE0\r\n", "OK", 500);                          //USART_DEBUG_SendStr("ATE0 OK \r\n");
  Omnivoltaic_GPRSCommand("AT+CIPCLOSE\r\n", "" ,100);                     //USART_DEBUG_SendStr("AT+CIPCLOSE OK \r\n");
  Omnivoltaic_GPRSCommand("AT+CIPSHUT\r\n", "OK" ,500);                    //USART_DEBUG_SendStr("AT+CIPSHUT OK \r\n");
  Omnivoltaic_GPRSCommand("AT+CPIN?\r\n", "OK", 500);                      //USART_DEBUG_SendStr("AT+CPIN? OK\r\n");
  Omnivoltaic_GPRSCommand("AT+CSQ\r\n", "OK", 500);                        //USART_DEBUG_SendStr("AT+CSQ OK \r\n");
  Omnivoltaic_GPRSCommand("AT+CREG?\r\n", "OK", 300);                      //USART_DEBUG_SendStr("AT+CREG? OK \r\n");
  Omnivoltaic_GPRSCommand("AT+CGATT?\r\n", "OK",300);                      //USART_DEBUG_SendStr("AT+CGATT? OK \r\n");
  //SYS_CHECK;
//  Omnivoltaic_GPRSCommand(APN, "OK", 150, 1, GPRSBUFFER);
  Omnivoltaic_GPRSCommand("AT+CSTT=\"CMNET\"\r\n", "OK", 300);             //USART_DEBUG_SendStr("APN OK \r\n");
  Omnivoltaic_GPRSCommand("AT+CSTT?\r\n", "OK", 300);                      //USART_DEBUG_SendStr("AT+CSTT? OK \r\n");
  Omnivoltaic_GPRSCommand("AT+CIICR\r\n", "OK", 500);                     //USART_DEBUG_SendStr("AT+CIICR OK \r\n");
  Omnivoltaic_GPRSCommand("AT+CIFSR\r\n", ".", 100);                       //USART_DEBUG_SendStr("AT+CIFSR OK \r\n");
//  Omnivoltaic_GPRSCommand("AT+CSQ\r\n", "OK", 100);
  Omnivoltaic_GPRSCommand(Defualt_URL, "CONNECT OK", 1000);                 //USART_DEBUG_SendStr("TCP CONNECT OK OK \r\n");

	//*/

	 return 1;
}


