#include <stdio.h>
#include <math.h>
#include "resid-visualizer.h"

ReSIDVisualizer::ReSIDVisualizer(ReSID *r, ReSIDPbData *d)
{
    R = r; 
    D = d;

    printf("[RSVIZ] ReSID visualizer initialized\n"); 
}

ReSIDVisualizer::~ReSIDVisualizer()
{
}

int ReSIDVisualizer::getNote(int freq)
{
    if(freq < 0x0117) return -1;

    unsigned char freqtbllo[] = {
      0x17,0x27,0x39,0x4b,0x5f,0x74,0x8a,0xa1,0xba,0xd4,0xf0,0x0e,
      0x2d,0x4e,0x71,0x96,0xbe,0xe8,0x14,0x43,0x74,0xa9,0xe1,0x1c,
      0x5a,0x9c,0xe2,0x2d,0x7c,0xcf,0x28,0x85,0xe8,0x52,0xc1,0x37,
      0xb4,0x39,0xc5,0x5a,0xf7,0x9e,0x4f,0x0a,0xd1,0xa3,0x82,0x6e,
      0x68,0x71,0x8a,0xb3,0xee,0x3c,0x9e,0x15,0xa2,0x46,0x04,0xdc,
      0xd0,0xe2,0x14,0x67,0xdd,0x79,0x3c,0x29,0x44,0x8d,0x08,0xb8,
      0xa1,0xc5,0x28,0xcd,0xba,0xf1,0x78,0x53,0x87,0x1a,0x10,0x71,
      0x42,0x89,0x4f,0x9b,0x74,0xe2,0xf0,0xa6,0x0e,0x33,0x20,0xff};

    unsigned char freqtblhi[] = {
      0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,
      0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x04,
      0x04,0x04,0x04,0x05,0x05,0x05,0x06,0x06,0x06,0x07,0x07,0x08,
      0x08,0x09,0x09,0x0a,0x0a,0x0b,0x0c,0x0d,0x0d,0x0e,0x0f,0x10,
      0x11,0x12,0x13,0x14,0x15,0x17,0x18,0x1a,0x1b,0x1d,0x1f,0x20,
      0x22,0x24,0x27,0x29,0x2b,0x2e,0x31,0x34,0x37,0x3a,0x3e,0x41,
      0x45,0x49,0x4e,0x52,0x57,0x5c,0x62,0x68,0x6e,0x75,0x7c,0x83,
      0x8b,0x93,0x9c,0xa5,0xaf,0xb9,0xc4,0xd0,0xdd,0xea,0xf8,0xff};

    int d;
    int nextfreq = 0x0000;
    int cmpfreq  = 0x0000;
    for (d = 0; d < 96; d++) {
        nextfreq = freqtbllo[d] | (freqtblhi[d] << 8);
        if(freq < nextfreq) {
            cmpfreq = freqtbllo[d-1] | (freqtblhi[d-1] << 8);
            cmpfreq = cmpfreq + (nextfreq - cmpfreq) / 2;
            if(freq > cmpfreq) return d;
            else return d-1;
        }
    }

    return -1;
}

void ReSIDVisualizer::visualize()
{
    if(!D || !D->buf_playing) return;

    char vu_color_str[16];
    vu_color_str[0] = 0x00;

    // VU-Meter val in v_val, color in vu_color_str
    short v_val = 0; 
    for(int i=1; i<CFG_AUDIO_BUF_SIZE; i++) {
        if(D->buf_playing[i] > v_val) v_val = D->buf_playing[i];
    }
    sprintf(vu_color_str, "\x1b[38;5;%dm", 235 + (v_val >> 11));

    // -- tests
    printf("\x1b[9A");       // 5 lines up
    printf("*\x1b[1D");      // 1 left
    printf("\x1b[9B");       // 5 lines down

    // -- Visualize oscillators
    printf("\x1b[38;5;59m"); // color
    printf("  |   |        |                        |\n");
    printf("\x1b[38;5;60m"); // color
    printf("  |OSC| CTL/WF | PULSEWIDTH             | FRQReg/Hz NOTE\n");
    for(int i=0; i<3; i++) {
        visualizeOsc(i + 1);
    }
    printf("\x1b[38;5;60m"); // color
    printf("  %c%c%c   |        |                        |\n",
           0xe2, 0x96, 0x80);
    printf("\x1b[38;5;59m"); // color
    printf("      %c%c%c        %c%c%c                        %c%c%c\n",
           0xe2, 0x96, 0x80,
           0xe2, 0x96, 0x80,
           0xe2, 0x96, 0x80
           );

    // -- MAIN Volume
    printf("\x1b[38;5;60m"); // color

    printf("  Main Out: %s%c%c%c\n",
           vu_color_str, 
           0xe2, 0x96, 0x81 + (v_val >> 12));
    
    printf("\x1b[38;5;238m"); // color
    printf("  Frame: %lu, Buffers Played: %lu, Underruns: %lu\n",
        D->stat_framectr, 
        D->stat_cnt,
        D->stat_buf_underruns
        );

    printf("\x1b[9A"); // go 6 lines up
    fflush(stdout);
}

// nr: 1...3
// returns: # of lines printed
int ReSIDVisualizer::visualizeOsc(int nr) 
{
    int  i;
    char bar16[17];
    bar16[16] = 0;

    const char *notename[] =
     {"C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "B-0",
      "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
      "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
      "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
      "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
      "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
      "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
      "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "B-7"};

    int osc_base = (nr-1) * 7;
    printf("\x1b[38;5;%dm", nr - 1 + 97); 
    // -- OSC CTRL bits, WF, PW
    printf("  | %d |", nr);
    if(R->shadow_regs[osc_base + 0x04] & 8) printf("T"); else printf(" ");
    if(R->shadow_regs[osc_base + 0x04] & 4) printf("R"); else printf(" ");
    if(R->shadow_regs[osc_base + 0x04] & 2) printf("S"); else printf(" ");
    if(R->shadow_regs[osc_base + 0x04] & 1) printf("G"); else printf(" ");
    printf(" %02x | %02x%02x ",
        R->shadow_regs[osc_base + 0x04],        // WF 1
        R->shadow_regs[osc_base + 0x03] & 0x0f, // PW 1 HI (4bit)
        R->shadow_regs[osc_base + 0x02]         // PW 1 LO (8bit)
        );

    // PW h-bar
    for(i=0;i<0x10;i++) {
        if( i < (R->shadow_regs[osc_base + 0x03] & 0x0f) ) bar16[i] = '*'; 
        else bar16[i] = ' ';
    }
    printf("%s", bar16);

    // PW v-bar (8 steps)
    int v_val = (R->shadow_regs[osc_base + 0x03] & 0x0f) >> 1;
    // U+2581 + v_val
    printf("%c%c%c | ", 0xe2, 0x96, 0x81 + v_val);

    // Frequency
    double f = ((R->shadow_regs[osc_base+1] << 8) + R->shadow_regs[osc_base]) / 
                17.02841924;
    printf("%02x%02x ", R->shadow_regs[osc_base+1], R->shadow_regs[osc_base]);
    if(f <  1000) printf(" ");
    if(f <   100) printf(" ");
    printf("%d  ", (int)f);

    int n=0;
    // printf("\x1b[38;5;60m"); // color
    printf("\x1b[38;5;%dm", nr - 1 + 127); 
    if((n=getNote((R->shadow_regs[osc_base+1] << 8) + R->shadow_regs[osc_base])) 
        >= 0) {
        printf("%s", notename[n]);
    } else {
        printf("   ");
    }

    printf("\n");
    return 1;
}

