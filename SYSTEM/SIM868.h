
#ifndef __SIM800_H
#define __SIM800_H


#include "stdio.h"	
#include "sys.h" 


u16 Omnivoltaic_GPRSCommand(char *cmd, char *answer, u16 wait_time);




u16 Omnivoltaic_GPRSConnctTcp(char* ipaddr  ,int port);



#endif

