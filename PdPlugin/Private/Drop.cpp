//
//  Drop.cpp
//  Pd_Index
//
//  Created by Esler,Robert Wadhams on 6/2/25.
//  Copyright © 2025 Epic Games, Inc. All rights reserved.
//

#include "VoltageControlFilter.h"
#include <vector>

using namespace pd;

class Drop {
private:
    VoltageControlFilter vcf;
    double signal = 0;
    double centerFreq;
    double rain = 0;
    double rainVol = 0;
    vcfOutput vcfOut;

public:
    Drop() {
        vcf.setQ(0.01);
    }

    double perform(double sig, double cf, double r, double rv) {
        double output = 0;
        vcfOut = vcf.perform(sig, cf);

        double x = max(clip(vcfOut.real, 0, 1), r);
        double y = x - r;
        double z = y * y;
        output = (z * z) * rv;

        return output;
    }

    // Emulates [clip~], ensuring value is within range
    double clip(double a, double b, double c) {
        if (a < b) return b;
        if (a > c) return c;
        return a;
    }

    // Emulates [max~], returning the higher of two values
    double max(double a, double b) {
        return (a > b) ? a : b;
    }
};

