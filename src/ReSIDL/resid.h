#ifndef SDL_RESID_H
#define SDL_RESID_H

#include "audio-config.h"
#include "resid/sid.h" // NOTE: <=============================== C++

typedef struct S_ReSIDPbData {
  short buf1[CFG_AUDIO_BUF_SIZE + 1];
  short buf2[CFG_AUDIO_BUF_SIZE + 1];
  short *buf_playing = 0;
  short *buf_next = 0;
  char buf_consumed = 0;
  char buf_lock = 0;
  char play = 0;
  unsigned long stat_cnt = 0;
  unsigned long stat_bufwrites = 0;
  unsigned long stat_buf_underruns = 0;
  unsigned long stat_framectr = 0;
} ReSIDPbData;

typedef struct S_ReSID_t {
  // public:
  //     ReSID();
  //     ~ReSID();
  //
  // void writeRegs(unsigned char *regs, int len);
  //
  // // dumb audio rendering, not frame aware
  // int clock(unsigned int cycles, short *buf, int buflen);
  //

  // calculated CONSTANTS
  int SAMPLES_PER_FRAME;
  int CYCLES_PER_FRAME;
  double CYCLES_PER_SAMPLE;

  unsigned char shadow_regs[32];

  // private:
  //     void precalc_constants();

  SID sid;    // NOTE:  <=================================== C++ obj
} ReSID;

typedef struct S_SID_Dump_Player_t {
  short outputs[3];

  ReSID *R;
  ReSIDPbData *D;

  // sid dmp
  unsigned char *dmp;
  unsigned int dmp_len;
  unsigned int dmp_idx;

  // audio buffer fill: samples until next frame
  int samples2do; // 882
} ReSIDDmpPlayer;

//
// class ReSID
// {
// public:
//     ReSID();
//     ~ReSID();
//
//     void writeRegs(unsigned char *regs, int len);
//
//     // dumb audio rendering, not frame aware
//     int clock(unsigned int cycles, short *buf, int buflen);
//
//     // calculated CONSTANTS
//     int SAMPLES_PER_FRAME;
//     int CYCLES_PER_FRAME;
//     double CYCLES_PER_SAMPLE;
//
//     unsigned char shadow_regs[32];
//
// private:
//     void precalc_constants();
//
//     SID sid;
// };

void ReSID_init(ReSID *resid);

void ReSID_writeRegs(ReSID *resid, unsigned char *regs, int len);

int ReSID_clock(ReSID *resid, unsigned int cycles, short *buf, int buflen);

void ReSID_precalc_constants(ReSID *resid);

#endif
