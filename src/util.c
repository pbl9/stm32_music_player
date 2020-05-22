#include "util.h"


void send_error(char* buffer,const char* file,uint16_t line,void (*sender)(char*,uint16_t)){
	sprintf(buffer,"\n\rError in %s at %d\n\r",file,(int)line);
	sender(buffer,strlen(buffer));//function that inform user about error using uart, display
	                              //at screen etc.
}
