#include "util.h"

void send_error(char* buffer,const char* file,uint16_t line,void (*sender)(char*,uint16_t)){
	sprintf(buffer,"\n\rError in %s at %d\n\r",file,(int)line);
	sender(buffer,strlen(buffer));//function that inform user about error using uart, display
	                              //at screen etc.
}
void fill_zeros(uint8_t* buffer,uint16_t begin,uint16_t end){
	for(uint16_t i=begin;i<end;i++){
		buffer[i]=0;
	}
}
