/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32l4xx.h"
#include "stm32l4xx_nucleo.h"
#include "main.h"
#include "util.h"

#define FATFS_MKFS_ALLOWED 0

FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
uint8_t workBuffer[_MAX_SS];
volatile uint8_t half_xfer_complete=0;
volatile uint8_t xfer_complete=0;

int main(void)
{
	FRESULT fr;
    DIR dir;
    FILINFO fno;
    FIL fil;
    uint16_t samples_buffer[SAMPLES_BUFF_SIZE];
    uint8_t temp_buff[2*SAMPLES_BUFF_SIZE]={0};
    uint32_t j=0;
    uint16_t buff_pointer=0;
    char buf[260];
    struct Header header;
	HAL_Init();
	SystemClock_Config();
	GPIO_Conf();
	UART_Conf();
	DMA_Init();
	DAC_Conf();
    Sample_Clock_Init();
	BSP_LED_Init(LED2);

	application_state_typedef application_state = IDLE;
	if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0){  /* 1- Link the micro SD disk I/O driver */
		if(BSP_SD_Init() == MSD_OK) /*-2- Init the SD Card*/
			application_state = INIT;
		else{
			send_error(buf,__FILE__,__LINE__,send_message);
			Error_Handler();
		  }
	  }
	  else{
		  send_error(buf,__FILE__,__LINE__,send_message);
		  Error_Handler();
	  }

	  while (1){
		  switch(application_state){
		  case INIT:
	    	if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK){
	    	    send_error(buf,__FILE__,__LINE__,send_message);
	    	    Error_Handler();
	    	}
	    	fr = f_findfirst(&dir, &fno, "", "*.wav");
	    	if(fr==FR_OK && fno.fname[0]){
	    		sprintf(buf,"%s\n" ,fno.fname);
	    		send_message(buf,strlen(buf));
	    		application_state=PLAYING;
	    	}
	    	else{
	    		send_error(buf,__FILE__,__LINE__,send_message);
	    		application_state=IDLE;
	    	}
	    	break;
		  case FIND:
			  fr = f_findnext(&dir,&fno);
			  if(fr==FR_OK && fno.fname[0]){
				  sprintf(buf,"%s\n" ,fno.fname);
				  send_message(buf,strlen(buf));
				  application_state=PLAYING;
			  }
			  else{
				  send_error(buf,__FILE__,__LINE__,send_message);
				  application_state=IDLE;
			  }
			  break;
		  case PLAYING:
			  if( (fr=f_open(&fil,fno.fname,FA_READ)) != FR_OK){
				  send_error(buf,__FILE__,__LINE__,send_message);
				  Error_Handler();
			  }
			  if( (fr=f_read(&fil,temp_buff,44,NULL))!= FR_OK ){
				  send_error(buf,__FILE__,__LINE__,send_message);
				  Error_Handler();
			  }
			  read_wav_frame(temp_buff,&header);
			  sprintf(buf,"Data about file:\n\rchannels:%d\n\rsample rate:%d\n\rbits_per_sample:%d\n\rdata size:%d\n\r" ,(int)header.channels,(int)header.sample_rate,(int)header.bits_per_sample,(int)header.data_size);
			  send_message(buf,strlen(buf));

			  fr=f_lseek(&fil,44);
			  fr=f_read(&fil,temp_buff,2*SAMPLES_BUFF_SIZE,NULL);
			  for(j=0;j<SAMPLES_BUFF_SIZE;j++){
				  samples_buffer[j]=getSamplesFromBytes(temp_buff+2*j);
			  }
			  buff_pointer=0;
			  extern TIM_HandleTypeDef tim;
			  HAL_TIM_Base_Start(&tim);
			  extern DAC_HandleTypeDef hdac1;
			  HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)samples_buffer,SAMPLES_BUFF_SIZE,DAC_ALIGN_12B_R);
			  UINT bytes_read=0;
			  uint32_t j=0;
			  while(f_eof(&fil)==0)
			  {
				  if(half_xfer_complete==1 && buff_pointer<SAMPLES_BUFF_SIZE/2){
					  BSP_LED_On(LED2);
					  fr=f_read(&fil,temp_buff,SAMPLES_BUFF_SIZE,&bytes_read);
					  if(bytes_read!=SAMPLES_BUFF_SIZE){
						  fill_zeros(temp_buff,(uint16_t)bytes_read,SAMPLES_BUFF_SIZE);
					  }
					  if(fr!=FR_OK){
						  send_error(buf,__FILE__,__LINE__,send_message);
						  Error_Handler();
					  }
					  for(j=0;j<SAMPLES_BUFF_SIZE/2;j++){
						  samples_buffer[buff_pointer]=getSamplesFromBytes(temp_buff+2*j);
						  buff_pointer++;
					  }
					  half_xfer_complete=0;
				  }
				  else if(xfer_complete==1 && buff_pointer<SAMPLES_BUFF_SIZE &&
						  buff_pointer >= SAMPLES_BUFF_SIZE/2){
					  fr=f_read(&fil,temp_buff,SAMPLES_BUFF_SIZE,&bytes_read);
					  if(fr!=FR_OK){
						  send_error(buf,__FILE__,__LINE__,send_message);
						  Error_Handler();
					  }
					  if(bytes_read!=SAMPLES_BUFF_SIZE){
						  fill_zeros(temp_buff,(uint16_t)bytes_read,SAMPLES_BUFF_SIZE);
					  }
					  for(j=0;j<SAMPLES_BUFF_SIZE/2;j++){
						  samples_buffer[buff_pointer]=getSamplesFromBytes(temp_buff+2*j);
						  buff_pointer++;
					  }
					  if(buff_pointer<=(SAMPLES_BUFF_SIZE-2)) buff_pointer++;
					  else buff_pointer=0;

					  xfer_complete=0;
				  }
				  else{
					  continue;
				  }
			  }
			  if(buff_pointer>=SAMPLES_BUFF_SIZE){
				  while(!half_xfer_complete) j++;
			  }
			  else{
				  while(!xfer_complete) j++;
			  }
			  HAL_DAC_Stop_DMA(&hdac1,DAC_CHANNEL_1);
			  BSP_LED_Off(LED2);
			  f_close(&fil);
			  application_state = FIND;
			  break;
		  case IDLE:
			  break;
		  case SD_UNPLUGGED:
			  application_state = IDLE;
			  break;
		  default:
			  break;
	    }
	}
}

