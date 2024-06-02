#ifndef RESID_DMPPLAYER_H
#define RESID_DMPPLAYER_H

#include "resid.h"

class ReSIDDmpPlayer
{
public:
    // instanciate with resid and playback data struct only
    ReSIDDmpPlayer(ReSID *r, ReSIDPbData *d);
    ~ReSIDDmpPlayer();

    // set sid dump to playback
    void setDmp(unsigned char *dump, unsigned int len);
    int  loadDmp(unsigned char *filename);

    // playback control
    void play(); // always start from beginning
    void stop(); // stops and resets playback data
    void pause();// stops generation of new audio buffers only
    void cont(); // continues updating audio buffer

    // continuously call this from outside, to ensure
    // audio buffer is filled and ready for playback
    // (compare to teensy audio library)
    // when called not often enough, buffer underrun will be 
    // detected
    // returns 1 on end of dump
    int update();

    short outputs[3];

private:
    int fill_audio_buffer();
    int set_next_regs(); // called on each frame by fill_audio_buffer

    ReSID *R;
    ReSIDPbData *D;

    // sid dmp
    unsigned char *dmp;
    unsigned int dmp_len;
    unsigned int dmp_idx;

    // audio buffer fill: samples until next frame
    int samples2do; // 882
};

#endif

