/*
 * main.h
 *
 *  Created on: Feb 19, 2019
 *      Author: pawel
 */

#ifndef MAIN_H_
#define MAIN_H_

#define SAMPLES_BUFF_SIZE 8192

#include "stm32l4xx_it.h"

#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "conf.h"
#include "wav_file.h"
#include <string.h>
#include <stdio.h>

typedef enum {
  IDLE = 0,
  INIT,
  FIND,
  PLAYING,
}application_state_typedef;

#endif /* MAIN_H_ */
