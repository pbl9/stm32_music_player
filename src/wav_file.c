/*
 * wave.c
 *
 *  Created on: Oct 23, 2019
 *      Author: pawel
 */

#include <wav_file.h>

uint16_t getSamplesFromBytes(uint8_t* bytes)
{
	int16_t sample_from_bytes=(bytes[1]<<8)|bytes[0];
	int32_t temp_sample=(int32_t)(sample_from_bytes+32768);
	uint16_t sample_for_dac=(uint16_t)(temp_sample>>4);//shift to 12 bits
	return sample_for_dac;
}
void wav_read_header(uint8_t* header_buff,struct wavHeader* wav_header)
{
	header_buff+=22;
	wav_header->channels=*header_buff|(*(header_buff+1)<<8);
	//get sampling rate
	header_buff+=2;
	wav_header->sample_rate=header_buff[0]|(header_buff[1]<<8)|(header_buff[2]<<16)|(header_buff[3]<<24);
	header_buff+=8;//fr=f_lseek(&fil,32);
	wav_header->block_align=header_buff[0]|(header_buff[1]<<8);
	header_buff+=2; //get bits per sample
	wav_header->bits_per_sample=header_buff[0]|(header_buff[1]<<8);
	header_buff+=6;
	wav_header->data_size=header_buff[0]|(header_buff[1]<<8)|(header_buff[2]<<16)|(header_buff[3]<<24);
}
void wav_write_about(char* buffer,struct wavHeader wav_header){
	sprintf(buffer,"\n\rAbout file:\n\rchannels:%d\n\rsample rate:%d\n\rbits per sample:%d\n\rdata size:%d\n\r",
			(int)wav_header.channels,(int)wav_header.sample_rate,(int)wav_header.bits_per_sample,
			(int)wav_header.data_size);
}
void wav_samples_from_bytes(uint8_t* bytes,uint16_t* samples,uint32_t number_of_samples){
	for(uint32_t i=0;i<number_of_samples;i++){
		*(samples++)=getSamplesFromBytes(bytes+2*i);
	}
}
