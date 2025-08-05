/*
 
All code not associated with the Unreal Engine API: Copyright © 2025 Robert Esler

 */

//Copyright © 2025 Epic Games, Inc. All rights reserved.


#include <iostream>
#include <cmath>
#include <cstdlib>
#include "Noise.h"
#include "LowPass.h"
#include "HighPass.h"
#include "Envelope.h"
#include "BandPass.h"
#include "Line.h"

using namespace pd;

class FireGen {
private:
    Noise noise;
    LowPass lop, lop2;
    HighPass hip, hip2, hip3;
    Envelope env;
    BandPass bp, bp2;
    Line line;

    double crackleVol = 0;
    double bpCf = 0;
    float time = 0;
    double vol = 0;
    float rand = 0;

public:
    FireGen() {
        line.setSampleRate(44100);
        lop.setCutoff(1);
        lop2.setCutoff(1);
        bp.setQ(1);
        bp.setCenterFrequency(4000);
        bp2.setQ(5);
        bp2.setCenterFrequency(30);
        hip.setCutOff(1000);
        hip2.setCutOff(25);
        hip3.setCutOff(25);
        std::srand(std::time(0));
    }

    double perform() {
        double output = 0;
        double n = noise.perform();
        double crack = crackles(n);
        double hiss = hissing(n);
        double lap = lapping(n);
        output = (crack * 0.2) + (hiss * 0.3) + (lap * 0.6);
        return output;
    }

private:
    double crackles(double input) {
        double output = 0;
        double low = lop.perform(input);
        double moses = env.perform(low);
        bool bang = false;

        if (moses >= 50.5 && moses <= 51) {
            rand = static_cast<float>(std::rand() % 30);
            bang = true;
            bpCf = (rand * 500) + 1500;
            time = rand;
        }
        if (bang) {
            line.perform(1, 1);
        }

        crackleVol = line.perform(0, time);
        vol = crackleVol * crackleVol;
        bp.setCenterFrequency(bpCf);
        output = bp.perform(input) * vol;

        return output;
    }

    double hissing(double input) {
        double output = 0;
        double tempV = lop2.perform(input) * 10;
        double vol = ((tempV * tempV) * (tempV * tempV)) * 600;
        output = hip.perform(input) * vol;
        return output;
    }

    double lapping(double input) {
        double output = 0;
        double clippedVal = hip2.perform(bp2.perform(input) * 100);
        double clipped = clip(clippedVal, -0.9, 0.9);
        output = hip3.perform(clipped) * 0.6;
        return output;
    }

    double clip(double a, double b, double c) {
        return (a < b) ? b : (a > c) ? c : a;
    }

};

