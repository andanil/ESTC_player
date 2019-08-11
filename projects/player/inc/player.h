#ifndef PLAYER_H
#define PLAYER_H

#include "song_list.h"
#include "player_internal.h"
#include "actions_indication.h"

typedef enum {
  AUDIO_MUTE_ON,
  AUDIO_MUTE_OFF 
} Commands_ID_t;

#define VolumeStep 5

void PlayFile(struct List *song, FRESULT fresult, uint16_t *begin_pos);
uint8_t OpenDir(struct List *first, struct List *last,  FRESULT fresult, char *path);
void PlayByteArray(uint16_t *start_pos, uint32_t size);
void Player_VolumeUp(void);
void Player_VolumeDown(void);
void Player_Toggle(void);
void Player_ChangeSong(void);

#endif /* PLAYER_H */