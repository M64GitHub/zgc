#ifndef RESID_DMPPLAYER_H
#define RESID_DMPPLAYER_H
#include "resid.h"
// // instanciate with resid and playback data struct only
// // ReSIDDmpPlayer_init(ReSID *r, ReSIDPbData *d);
//
// // set sid dump to playback
// void setDmp(unsigned char *dump, unsigned int len);
// int loadDmp(unsigned char *filename);
//
// // playback control
// void play();  // always start from beginning
// void stop();  // stops and resets playback data
// void pause(); // stops generation of new audio buffers only
// void cont();  // continues updating audio buffer
//
// // continuously call this from outside, to ensure
// // audio buffer is filled and ready for playback
// // (compare to teensy audio library)
// // when called not often enough, buffer underrun will be
// // detected
// // returns 1 on end of dump
// int update();
//
// int fill_audio_buffer();
// int set_next_regs(); // called on each frame by fill_audio_buffer

void ReSIDDmpPlayer_ReSIDDmpPlayer_init(ReSIDDmpPlayer *dp, ReSID *r,
                                        ReSIDPbData *d);

void ReSIDDmpPlayer_setDmp(ReSIDDmpPlayer *dp, unsigned char *dump,
                           unsigned int len);

int ReSIDDmpPlayer_update(ReSIDDmpPlayer *dp);

void ReSIDDmpPlayer_play(ReSIDDmpPlayer *dp);

void ReSIDDmpPlayer_stop(ReSIDDmpPlayer *dp);

void ReSIDDmpPlayer_pause(ReSIDDmpPlayer *dp);

void ReSIDDmpPlayer_cont(ReSIDDmpPlayer *dp);

int ReSIDDmpPlayer_set_next_regs(ReSIDDmpPlayer *dp);

int ReSIDDmpPlayer_fill_audio_buffer(ReSIDDmpPlayer *dp);

#endif
