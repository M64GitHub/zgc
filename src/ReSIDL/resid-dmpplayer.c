#include "resid-dmpplayer.h"
#include <stdio.h>

void ReSIDDmpPlayer_ReSIDDmpPlayer_init(ReSIDDmpPlayer *dp, ReSID *r,
                                        ReSIDPbData *d) {
  dp->R = r;
  dp->D = d;

  dp->dmp = 0;
  dp->dmp_len = 0;
  dp->samples2do = 0;

  printf("[DMPPl] ReSID dump player initialized\n");
}

void ReSIDDmpPlayer_setDmp(ReSIDDmpPlayer *dp, unsigned char *dump,
                           unsigned int len) {
  dp->dmp = dump;
  dp->dmp_len = len;
}

// call this frequently, to never underrun audio buffer fill
int ReSIDDmpPlayer_update(ReSIDDmpPlayer *dp) {
  if (!dp->D->buf_consumed)
    return 0;
  if (ReSIDDmpPlayer_fill_audio_buffer(dp))
    return 1; // end of dmp reached
  dp->D->buf_consumed = 0;

  return 0;
}

void ReSIDDmpPlayer_play(ReSIDDmpPlayer *dp) {
  if (!dp->dmp || !dp->dmp_len)
    return;

  // prepare buffer for first playback
  dp->dmp_idx = 0;
  dp->D->buf_playing = 0;
  dp->D->buf_next = dp->D->buf1;
  ReSIDDmpPlayer_set_next_regs(dp);
  dp->samples2do = dp->R->SAMPLES_PER_FRAME;
  ReSIDDmpPlayer_fill_audio_buffer(dp);
  dp->D->buf_lock = 0;

  // start audio playback
  dp->D->play = 1;
}

void ReSIDDmpPlayer_stop(ReSIDDmpPlayer *dp) {
  dp->D->play = 0;
  dp->dmp_idx = 0;
}

void ReSIDDmpPlayer_pause(ReSIDDmpPlayer *dp) { dp->D->play = 0; }

void ReSIDDmpPlayer_cont(ReSIDDmpPlayer *dp) { dp->D->play = 1; }

int ReSIDDmpPlayer_set_next_regs(ReSIDDmpPlayer *dp) {
  // dmp format stores 25 reg vals
  int numregs = 25;

  if (!dp->dmp || !dp->dmp_len)
    return 2;
  if ((dp->dmp_idx + numregs) > dp->dmp_len)
    return 1;

  ReSID_writeRegs(dp->R, dp->dmp + dp->dmp_idx, numregs);
  dp->dmp_idx += numregs;

  return 0;
}

int ReSIDDmpPlayer_fill_audio_buffer(ReSIDDmpPlayer *dp) {
  int bufpos = 0;
  int remainder = 0;
  int cycles2do = 0;
  ;

  dp->D->buf_lock = 1;

  while ((bufpos + dp->samples2do) < CFG_AUDIO_BUF_SIZE) {
    cycles2do = (dp->R->CYCLES_PER_SAMPLE * dp->samples2do + 0.5);
    // printf("[DMPPl] frame: %lu, samples2do: %d, cycles2do: %d, ctr: %lu\n",
    //        D->stat_framectr,
    //        samples2do, cycles2do, D->stat_cnt);
    ReSID_clock(dp->R, cycles2do, dp->D->buf_next + bufpos, CFG_AUDIO_BUF_SIZE);
    bufpos += dp->samples2do;

    // next frame
    // printf("[DMPPl] *** next frame ***\n");
    dp->D->stat_framectr++;
    dp->samples2do = dp->R->SAMPLES_PER_FRAME;
    if (ReSIDDmpPlayer_set_next_regs(dp))
      return 1; // end of dmp reached
  }

  remainder = CFG_AUDIO_BUF_SIZE - bufpos;
  cycles2do = ((double)remainder * dp->R->CYCLES_PER_SAMPLE + 0.5);
  // printf("[DMPPl] remainder: %d, bufpos: %d, cycles2do: %d\n",
  //        remainder, bufpos, cycles2do);
  ReSID_clock(dp->R, cycles2do, dp->D->buf_next + bufpos, CFG_AUDIO_BUF_SIZE);
  dp->samples2do -= remainder;
  bufpos = 0;

  dp->D->buf_lock = 0;
  return 0;
}
