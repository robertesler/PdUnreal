//
//  Rain.cpp
//  Pd_Index
//
//  Created by Esler,Robert Wadhams on 6/2/25.
//  Copyright © 2025 Epic Games, Inc. All rights reserved.
//

#include "Noise.h"
#include "BandPass.h"
#include "HighPass.h"
#include "LowPass.h"
#include "Delay.h"
#include "Drop.cpp"
#include "GlassWindow.cpp"
#include "Reverb.cpp"

using namespace pd;

class Rain {
    
private:
    Noise noise;
    BandPass bp;
    HighPass hip;
    LowPass lop;
    pd::Delay del; //This must a conflict with Metasound or something
    Drop drop;
    GlassWindow window;
    Reverb reverb;

    double delread = 0;
    double rain = 0.1;
    double rainVol = 3;
    double bpCf = 1400;
    double bpQ = 2;
    double bpRainVol = 2.8;

public:
    Rain() {
        del.setDelayTime(300);
        hip.setCutOff(9000);
        lop.setCutoff(500);
        bp.setCenterFrequency(bpCf);
        bp.setQ(bpQ);
    }

    // Main DSP processing
    double perform() {
        double outputL = 0, outputR = 0;
        double dropInput = (del.perform(delread) * 24) + 6;
        double n = noise.perform();
        double farFieldRain = bp.perform(n) * (bpRainVol * 0.01);
        double x = drop.perform(n, dropInput, rain, rainVol);
        double win = hip.perform(x) * 20;
        delread = window.perform(win);
        double combo = delread + farFieldRain;
        double r = reverb.perform(combo);
        outputR = lop.perform(delread) + combo + r * 0.4;
        return (outputL + outputR) * 2;
    }

    void setRain(float r, float rv, float bpf, float q, float ffrv) {
        rain = r;
        rainVol = rv;
        bpCf = bpf;
        bpQ = q;
        bpRainVol = ffrv;
    }

};
