#include "sys.h"
#include "usart3.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	




//Uart4.InitUart=0;
	
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 0
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 

u16  GetHeadByte(void);
void AddTailByte(u8 ch);



 /*
CircleLoopStruct Uart3ClsBuff=
{
	0,//_num
	0,//_head
	0,//_tail	
  &GetHeadByte,//GetHeadByte 	
	&AddTailByte,//AddByte
	{0}//_Rxbuf	 
};
*/

void ClearRxBuff(void)
{
	__disable_irq();	
  Uart3._RxHead=0;
	Uart3._RxNum=0;
	Uart3._RxTail=0;
  __enable_irq();
	
}

u16  GetHeadByte(void)
{
	  u16 ch=0;
	
    __disable_irq();	////
	 
	  if(Uart3._RxNum!=0) {	
			ch= Uart3._Rxbuf[Uart3._RxHead];
			Uart3._Rxbuf[Uart3._RxHead]=255;//make a lable
		
			Uart3._RxHead++;
			Uart3._RxNum-- ;
		
			if(Uart3._RxHead>=USART3_REC_LEN)  Uart3._RxHead=0;
	
	  }
		else {
	      ch=  0x8000;//empty
		}
		
    __enable_irq();/////
		
	  return ch;
	
}

void AddTailByte(u8 ch)
{
	/*
__disable_irq();		
	
	  if(Uart3._tail>=USART3_REC_LEN)  Uart3._tail=0;	
	
	  Uart3._Rxbuf[Uart3._tail]=ch;
	
	  Uart3._tail++;
	  Uart3._num++ ; 
	  if(Uart3._num>=USART3_REC_LEN)Uart3._num=USART3_REC_LEN;
	
__enable_irq();	 
	*/
}


#if EN_USART3_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   

void Init2(u32 baud);

int  SendData(u8* buf,u16 buflen,u16 timeout );
int  Receive(u8* buf,u16 len,u16 timeout );

 
UART_FUNCTION  Uart3={

		&Init2,
		&SendData,
		&Receive,	
		
		0,   //_num
		0,   //_head
		0,   //_tail	
		&GetHeadByte,//GetHeadByte 	
		&AddTailByte,//AddByte
	  &ClearRxBuff,//ClearBuff
		{0}//_Rxbuf	 

};//


//int  USART_DEBUG_SendStr(u8* buf,u16 buflen,u16 timeout )
/****************************************************************************************************
*函数
*topicstr 
*
*
****************************************************************************************************/
void USART_DEBUG_SendStr1(char * str)
{//for debug
	int i=0;
	int len=0;
	
	len=strlen(str);
	
	for(i=0;i<len;i++){
	
				while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
				USART1->DR = (u8) str[i];  				
	}

}

/****************************************************************************************************
*函数
*topicstr 
*
*
****************************************************************************************************/
int  Uart1_SendData2(u8* buf,u16 buflen,u16 timeout )
{//for DEBUG
		u16 i=0;

		while( i<buflen){
				while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
				USART1->DR = (u8) buf[i];  		
				i++;
		}
		return i;
}
/****************************************************************************************************
*函数
*topicstr 
*
*
****************************************************************************************************/
int  SendData(u8* buf,u16 buflen,u16 timeout )
{
		u16 i=0;

		while( i<buflen){
				while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
				USART3->DR = (u8) buf[i];  		   
			  USART1->DR =(u8) buf[i]; //reflect to uart1 for debug 	
			
				i++;
		}
		
		//for debug
		//Uart1_SendData2(buf,buflen,timeout);
		
		return i;
}
/****************************************************************************************************
*函数
*topicstr 
*
*
****************************************************************************************************/
int  Receive(u8* buf,u16 len,u16 timeout )
{
		u16 i=0;
	  u16 cc=0;
    u16 cnt=0; 
	 
		while(i<len)
		{
				//等待接收缓冲区有数据
				while( Uart3._RxNum==0){// //wait but  com sitll rcv 
						//if(((timeout_cnt++)>>10)>timeout){	 return -1;}	
					
					  delay_ms(1);
					  cnt++;
					  if(cnt>timeout) return -1;
				}
	
				cc=	 Uart3.GetHeadByte();
				buf[i]=cc;
				i++;

		}
		
		//for debug
		//Uart1_SendData2(buf,len,timeout);		
		
		return len;		
}

char  tmpbuf[20]={0};//"AT+CIPSEND=  57\r\n";
/****************************************************************************************************
*函数
*topicstr 
*
*
****************************************************************************************************/
int  FindStrFormSim800cResponds(char* str,int clr_rxbuf,u16 waitimes)
{	
	  char* fstart=0;
	
    if(clr_rxbuf){
	      Uart3.ClearRxBuff();
		    memset(Uart3._Rxbuf,0,30);
		}
		
		
 		delay_ms(waitimes);   		

	  memcpy(tmpbuf,Uart3._Rxbuf,20   ); 
	
		for(int i=0;i<20;i++){
			if(tmpbuf[i]==0)tmpbuf[i]=1;
		}
		tmpbuf[19]=0;
		
		
		if(str==NULL) return 0;
		
		
		fstart=strstr((const char *)tmpbuf,str ) ;	
		
	  if( fstart ==NULL)//没有找到 ""
		{
     	  return 0;		
		}		
    else
		{//找到之后，从接收缓冲区删除 被查找的字符串
			
			 // int len =strlen(str);
			 
			//int lent=sprintf(tmpbuf,"del %s",str);
			
			// Uart1_SendData2((u8*)tmpbuf,lent,10);
			
			  int num =tmpbuf-fstart;
        if(num<0)num=-num;			
         
			  int len =strlen(str);
			
			  Uart3._RxHead += num+len;
			  Uart3._RxNum  -= num+len;
			
        return 1;
		}
}	
/****************************************************************************************************
*函数
*topicstr 
*
*
****************************************************************************************************/
int  SendDataToSim800c(u8* buf,u16 buflen,u16 timeout)
{
	  int re=0;
	  int repeate_times=0;
	  volatile int ok=0;
    //send 	"AT+CIPSEND=<length>\r\n"  to SIM800  and wait  "SEND OK\r\n"
	  int len=sprintf(tmpbuf,"AT+CIPSEND=%d\r\n",buflen);
	
	  Uart3.SendData((u8*)tmpbuf,len,17); 
		//wait SIM800 responds ">"		
    if(ok==0)ok=FindStrFormSim800cResponds(">",0,500);
    if(ok==0)ok=FindStrFormSim800cResponds(">",0,500);
    if(ok==0)ok=FindStrFormSim800cResponds(">",0,500);	
    if(ok==0)ok=FindStrFormSim800cResponds(">",0,500);	
    if(ok==0) 
			return 0;
	
	  //then Send the real data;
	  re=Uart3.SendData(buf,buflen,1000); 

		
		//wait SIM800 responds "SEND OK\r\n"	
		ok=0;	
		repeate_times=50;
		
    if(ok==0)
			ok=FindStrFormSim800cResponds("SEND OK\r\n",1,100);		
		

		while(repeate_times--)
		{
				if(ok==0)
					ok=FindStrFormSim800cResponds("SEND OK\r\n",0,100);	
        else break;					
	  }
		
		if(ok==0) 
			return 0 ;
		
	 // Uart3.ClearRxBuff();
		
		return re;
	
}

/****************************************************************************************************
*函数
*topicstr 
*
*
****************************************************************************************************/
void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		      //Res =USART_ReceiveData(USART3);	//读取接收到的数据
					Res =(uint16_t)(USART3->DR & (uint16_t)0x01FF);
			
			    USART1->DR = Res;  //reflect to uart1 for debug 		
			
			    //如果链表到末尾，则折返链表头部，覆盖其他数据，继续接收
			    if(Uart3._RxTail>=USART3_REC_LEN){  
					    Uart3._RxTail=0;
					}
			
					Uart3._Rxbuf[Uart3._RxTail]=Res ; 
          Uart3._RxTail++;					
					Uart3._RxNum++;
					//如果接收长度大于链表总长，  
					if(Uart3._RxNum>USART3_REC_LEN){
					    Uart3._RxNum=USART3_REC_LEN;
					}

     } //
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
/****************************************************************************************************
*函数
*topicstr 
*
*
****************************************************************************************************/
void Init2(u32 baud)
{
 
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//USART1_TX   GPIOB.10
  gpioInitStruct.GPIO_Pin = GPIO_Pin_10; //PB.10
  gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//??????
  GPIO_Init(GPIOB, &gpioInitStruct);//???GPIOB.10
   
//USART1_RX	  GPIOB.11 
  gpioInitStruct.GPIO_Pin = GPIO_Pin_11;//PB.11
  gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//????
  GPIO_Init(GPIOB, &gpioInitStruct);//???GPIOB.11 
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//?????
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				//?????
	usartInitStruct.USART_Parity = USART_Parity_No;						//???
	usartInitStruct.USART_StopBits = USART_StopBits_1;					//1????
	usartInitStruct.USART_WordLength = USART_WordLength_8b;					//8????
	USART_Init(USART3, &usartInitStruct);
	
	USART_Cmd(USART3, ENABLE);														//????
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);						//??????
	
	nvicInitStruct.NVIC_IRQChannel = USART3_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);
	

}
 
void testloop()
{
//	u16  ok=0;
	/*
	Uart3ClsBuff.AddTailByte(1);
	Uart3ClsBuff.AddTailByte(2);	
	Uart3ClsBuff.AddTailByte(3);
	Uart3ClsBuff.AddTailByte(4);

	Uart3ClsBuff.AddTailByte(5);
	Uart3ClsBuff.AddTailByte(6);	
	Uart3ClsBuff.AddTailByte(7);
	Uart3ClsBuff.AddTailByte(8);
	
	Uart3ClsBuff.AddTailByte(9);
	Uart3ClsBuff.AddTailByte(0xa);	
//	Uart3ClsBuff.AddTailByte(0xb);
//	Uart3ClsBuff.AddTailByte(0xc);	
	
	ok= Uart3ClsBuff.GetHeadByte();
	
	ok= Uart3ClsBuff.GetHeadByte();	
	ok= Uart3ClsBuff.GetHeadByte();	
	ok= Uart3ClsBuff.GetHeadByte();	
	ok= Uart3ClsBuff.GetHeadByte();
	ok= Uart3ClsBuff.GetHeadByte();
	ok= Uart3ClsBuff.GetHeadByte();
	ok= Uart3ClsBuff.GetHeadByte();
	ok= Uart3ClsBuff.GetHeadByte();
	
	ok= Uart3ClsBuff.GetHeadByte();	
	ok= Uart3ClsBuff.GetHeadByte();


	Uart3ClsBuff.AddTailByte(1);
	Uart3ClsBuff.AddTailByte(2);	
	Uart3ClsBuff.AddTailByte(3);
	Uart3ClsBuff.AddTailByte(4);

	ok= Uart3ClsBuff.GetHeadByte();	
	ok= Uart3ClsBuff.GetHeadByte();
	*/
}


#endif	



#if 0
//============================================================================================================
//

void Uart3_Init(u32 bound)

{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3|RCC_APB2Periph_GPIOB, ENABLE);	//使能USART1，GPIOB时钟
              	        // RCC_APB1Periph_USART3
	
	//USART1_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10   PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOB.11初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11  PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口1 

}




#endif

