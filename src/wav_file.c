/*
 * wave.c
 *
 *  Created on: Oct 23, 2019
 *      Author: pawel
 */

#include <wav_file.h>

uint16_t getSamplesFromBytes(uint8_t* bytes)
{
	int16_t sample_from_bytes=0;
	uint16_t sample_for_dac=0;
	int32_t temp_sample=0;
	sample_from_bytes=(bytes[1]<<8)|bytes[0];
	temp_sample=(int32_t)(sample_from_bytes+32768);
	sample_for_dac=(uint16_t)(temp_sample>>4);
	return sample_for_dac;
}
void fill_zeros(uint8_t* buffer,uint16_t begin,uint16_t end)
{
	uint16_t i=0;
	for(i=begin;i<end;i++)
	{
		buffer[i]=0;
	}
}
void read_frame(uint8_t* header_buff,struct Header* wav_header)
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
