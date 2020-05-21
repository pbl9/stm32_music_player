/*
 * wave.h
 *
 *  Created on: Mar 21, 2019
 *      Author: pawel
 */

#ifndef WAV_FILE_H_
#define WAV_FILE_H_

#include "stm32l4xx_hal.h"

struct Header {
	uint16_t channels;
	uint32_t sample_rate;
	uint16_t block_align;//BitsPerSample * Channels 1 – 8 bit mono;2 – 8 bit stereo/16 bit mono;4 – 16 bit stereo
	uint16_t bits_per_sample;
	uint32_t data_size;
};

uint16_t getSamplesFromBytes(uint8_t* bytes);
void fill_zeros(uint8_t* buffer,uint16_t begin,uint16_t end);
void read_wav_frame(uint8_t* header_buff,struct Header* wav_header);

#endif /* WAV_FILE_H_ */
