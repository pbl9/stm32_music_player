#include "file_operations.h"

void file_read_from(FIL* file,uint8_t* buffer,uint16_t bytes_to_read,UINT* bytes_read){
	FRESULT fr=f_read(file,buffer,bytes_to_read,bytes_read);
	if(fr!=FR_OK){ Error_Handler(); }
}
