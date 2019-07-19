#ifndef PLAYER_H
#define PLAYER_H

#include "song_list.h"
#include "periph.h"
#include "actions_indication.h"

typedef enum {
  AUDIO_PAUSE,
  AUDIO_NEXT,  
  AUDIO_VOLUME_UP,
  AUDIO_VOLUME_DOWN,
  AUDIO_MUTE_ON,
  AUDIO_MUTE_OFF 
} Commands_ID_t;

#define VolumeStep 5

void TIM5_Init(void);
void PlayFile(struct List *song, FRESULT fresult);
void TIM5_IRQHandler(void);
uint8_t OpenDir(struct List *first, struct List *last,  FRESULT fresult, const char *path);

#endif /* PLAYER_H */