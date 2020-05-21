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

#define FATFS_MKFS_ALLOWED 0

FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
static uint8_t isInitialized = 0;
//static uint8_t isCreated = 0;
uint8_t workBuffer[_MAX_SS];
volatile uint8_t half_xfer_complete=0;
volatile uint8_t xfer_complete=0;

static void FS_FileOperations(void);
static void SD_Initialize(void);
FILINFO find_wav_file(DIR* direc);

int main(void)
{
    FRESULT fr;
    DIR dj;
    FILINFO fno;
    FIL fil;
    uint16_t samples_buffer[SAMPLES_BUFF_SIZE];
    uint8_t temp_buff[2*SAMPLES_BUFF_SIZE]={0};
    uint32_t j=0;
    uint16_t buff_pointer=0;
    char buf[260];
    struct Header header;
    const char* error_msg="f_read error";
	HAL_Init();
	SystemClock_Config();
	GPIO_Conf();
	UART_Conf();
	DMA_Init();
	DAC_Conf();
    Sample_Clock_Init();
	BSP_LED_Init(LED2);

	  /* 1- Link the micro SD disk I/O driver */
	application_state_typedef application_state = IDLE;
	  if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0){
	    /*##-2- Init the SD Card #################################################*/
	    SD_Initialize();
	    //if(BSP_SD_IsDetected())
	    //{
	      application_state = INIT;
	   // }
	  }
	  else
	  {
	    Error_Handler();
	    while(1) {};
	  }
	  while (1){
	    /* Mass Storage Application State Machine */
	    switch(application_state){
	    case INIT:
	    	SD_Initialize();
	    	FS_FileOperations();
	    	fr = f_findfirst(&dj, &fno, "", "*.wav");
	    	if(fr==FR_OK && fno.fname[0]){
	    		sprintf(buf,"%s\n" ,fno.fname);
	    		send_message(buf,strlen(buf));
	    		application_state=PLAYING;
	    	}
	    	else{
	    		application_state=IDLE;
	    	}
	    	break;
	    case FIND:
	    	fr = f_findnext(&dj,&fno);
	    	if(fr==FR_OK && fno.fname[0]){
        	  sprintf(buf,"%s\n" ,fno.fname);
        	  send_message(buf,strlen(buf));
        	  application_state=PLAYING;
          }
          else{
        	  application_state=IDLE;
          }
          break;
	    case PLAYING:
          if( (fr=f_open(&fil,fno.fname,FA_READ)) != FR_OK){
        	  Error_Handler();
          }
          if( (fr=f_read(&fil,temp_buff,44,NULL))!= FR_OK ){
        	  Error_Handler();
          }
          read_wav_frame(temp_buff,&header);
          sprintf(buf,"Data about file:\n\rchannels:%d\n\rsample rate:%d\n\rbits_per_sample:%d\n\rdata size:%d" ,(int)header.channels,(int)header.sample_rate,(int)header.bits_per_sample,(int)header.data_size);
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
        	  if(half_xfer_complete==1 && buff_pointer<SAMPLES_BUFF_SIZE/2)
        	  {
        		  BSP_LED_On(LED2);
            	  fr=f_read(&fil,temp_buff,SAMPLES_BUFF_SIZE,&bytes_read);
            	  if(bytes_read!=SAMPLES_BUFF_SIZE)
            	  {
            		  fill_zeros(temp_buff,(uint16_t)bytes_read,SAMPLES_BUFF_SIZE);
            	  }
            	  if(fr!=FR_OK)
            	  {
            		  send_message(error_msg,strlen(error_msg));
            	  }
            	  for(j=0;j<SAMPLES_BUFF_SIZE/2;j++)
            	  {
            		  samples_buffer[buff_pointer]=getSamplesFromBytes(temp_buff+2*j);
            		  buff_pointer++;
            	  }
            	  half_xfer_complete=0;
        	  }
        	  else if(xfer_complete==1 && buff_pointer<SAMPLES_BUFF_SIZE && buff_pointer >= SAMPLES_BUFF_SIZE/2)
        	  {
            	  fr=f_read(&fil,temp_buff,SAMPLES_BUFF_SIZE,&bytes_read);
            	  if(fr!=FR_OK){
            		  send_message(error_msg,strlen(error_msg));
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
        	  while(!half_xfer_complete)
        		  j++;
          }
          else{
        	  while(!xfer_complete)
        		  j++;
          }
          HAL_DAC_Stop_DMA(&hdac1,DAC_CHANNEL_1);
          f_close(&fil);
	      application_state = FIND;
	      break;
	    case IDLE:
	    	break;
	    case SD_UNPLUGGED:
	    	if (isInitialized == 1){
	        //Error_Handler();
	        isInitialized = 0;
	    	}
	    	application_state = IDLE;
	    	break;
	    default:
	    	break;
	    }
	  }
}
static void SD_Initialize(void)
{
  if (isInitialized == 0)
  {
    if (BSP_SD_Init() == MSD_OK)
    {
      isInitialized = 1;
    }
    else
    {
      BSP_LED_On(LED2);
      /* wait until the uSD is plugged */
      //while (BSP_SD_IsDetected() != SD_PRESENT)
      {}
      BSP_LED_Off(LED2);
    }
  }
}
static void FS_FileOperations(void)
{
  FRESULT res; /* FatFs function common result code */
  uint32_t byteswritten, bytesread; /* File write/read counts */
  uint8_t wtext[] = "linux stm32"; /* File write buffer */
 // uint8_t rtext[100]; /* File read buffer */

  /* Register the file system object to the FatFs module */
  if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) == FR_OK)
  {
#if FATFS_MKFS_ALLOWED
    if (isCreated == 0)
    {
      res = f_mkfs(SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer));

      if (res != FR_OK)
      {
        Error_Handler();
        while(1);
      }
    }
    isCreated = 1;
#endif


  }
  /* Error */
  //Error_Handler();
}
FILINFO find_wav_file(DIR* direc){
	FRESULT fr;
	FILINFO fno;
	fr = f_findnext(direc,&fno);
	if(fr != FR_OK)
		Error_Handler();
	return fno;
}

