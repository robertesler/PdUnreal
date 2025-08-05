/*
 A Schroeder Reverberator
 Four comb filters in parallel
 and two allpass filters in series
 
 All code not associated with the Unreal Engine API: Copyright © 2025 Robert Esler
 */
//  Copyright © 2025 Epic Games, Inc. All rights reserved.


#include "VariableDelay.h"
#include "LowPass.h"

using namespace pd;

class Reverb {
    
private:
    VariableDelay delay1, delay2, delay3, delay4, delay5, delay6;
    LowPass lop;

    double gain1 = 0.7, gain2 = 0.68, gain3 = 0.63, gain4 = 0.58;
    double gain5 = 0.77, gain6 = 0.79;
    double del1 = 0, del2 = 0, del3 = 0, del4 = 0;
    double allpass1 = 0, allpass2 = 0;
    
    double volume = 0.0;      // 0-100
    double crossover = 5000;  // Cutoff frequency
    double out = 0.0;         // Output signal

public:
    double perform(double input) {
        // Apply feedback comb filters
        delay1.delayWrite(del1 * gain1);
        del1 = delay1.perform(104) + input;

        delay2.delayWrite(del2 * gain2);
        del2 = delay2.perform(113) + input;

        delay3.delayWrite(del3 * gain3);
        del3 = delay3.perform(131) + input;

        delay4.delayWrite(del4 * gain4);
        del4 = delay4.perform(142) + input;

        double combs = (del1 + del2 + del3 + del4) * 0.25;  // Sum and normalize
        
        // Allpass filter series
        delay5.delayWrite(allpass1 * gain5);
        allpass1 = delay5.perform(23) + combs;
        allpass1 *= 1 - (gain5 * gain5);
        allpass1 += (gain5 * -1) * combs;

        delay6.delayWrite(allpass2 * gain6);
        allpass2 = delay6.perform(7.6) + allpass1;
        allpass2 *= 1 - (gain6 * gain6);
        allpass2 += (gain6 * -1) * allpass1;

        out = allpass1 + allpass2;

        // Apply low-pass filter
        lop.setCutoff(getCrossover());
        return lop.perform(out);
    }

    void setTime() {}  // Placeholder

    void setVolume(double v) { volume = v; }
    double getVolume() const { return volume; }

    void setCrossover(double c) { crossover = c; }
    double getCrossover() const { return crossover; }

   
};
