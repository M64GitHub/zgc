
#ifndef RESID_VISUALIZER_H
#define RESID_VISUALIZER_H

#include "resid.h"

class ReSIDVisualizer
{
public:
    ReSIDVisualizer(ReSID *r, ReSIDPbData *d);
    ~ReSIDVisualizer();

    void visualize();
    int  visualizeOsc(int nr); // returns # of lines

    int getNote(int freq);

private:
    ReSID *R;
    ReSIDPbData *D;
};

#endif
