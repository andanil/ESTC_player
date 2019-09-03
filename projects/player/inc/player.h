#ifndef PLAYER_H
#define PLAYER_H

#include "song_list.h"
#include "player_internal.h"
#include "actions_indication.h"
#include "wave_file.h"
#include "usbh_usr.h"


#define MEDIA_IntFLASH 
//#define MEDIA_USB_KEY  

typedef enum {
  AUDIO_MUTE_ON,
  AUDIO_MUTE_OFF 
} Commands_ID_t;

#define VolumeStep 5

void PlayFile(char *file_name);
void Player_VolumeUp(void);
void Player_VolumeDown(void);
void Player_Stop(void);
void Player_Toggle(void);
void Player_ChangeSong(void);
void PlayByteArray(uint16_t *begin_pos, uint32_t size);
void Player_TransferComplete_CallBack(uint16_t *begin_pos, uint32_t size);
uint8_t OpenDir(struct List *first, struct List *last,  FRESULT fresult, char *path);

#endif /* PLAYER_H */