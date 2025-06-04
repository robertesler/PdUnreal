/*
 Implementation of Andy Farnell's cricket example
 
 All code not associated with the Unreal Engine API: Copyright © 2025 Robert Esler
 */
//  Copyright © 2025 Epic Games, Inc. All rights reserved.

#include <iostream>
#include <cmath>
#include "Phasor.h"
#include "Cosine.h"

using namespace pd;

class CricketGen {
private:
    Phasor phasor;
    Cosine cos1;
    Cosine cos2;
    Cosine cos3;

public:
    double perform(double rate) {
        double r = phasor.perform(rate);
        double x = cos1.perform(r * 40.6);
        x *= x;  // Square the value

        double y = cos2.perform(r * 3147) + (cos3.perform(r * 3147 * 2) * 0.3);
        double vol = x * y;

        double a = std::min(r, 0.1714) * 5.84;
        double b = wrap(a) - 0.5;
        double c = ((b * b) * -4) + 1;
        double output = c * vol;

        return output * 0.1;
    }

    static double wrap(double input) {
        return fmod(input, 1.0);
    }
};
