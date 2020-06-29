#ifndef FILE_OPERATIONS_H_
#define FILE_OPERATIONS_H_
#include "main.h"

FILINFO find_next_file(DIR* direc);
void file_read_from(FIL* file,uint8_t* buffer,uint16_t bytes_to_read,UINT* bytes_read);

#endif
