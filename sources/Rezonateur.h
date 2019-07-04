#pragma once
#include "svf/VAStateVariableFilter.h"
#include <complex>

class Rezonateur {
public:
    void init(double samplerate);

    double process(double input);

    std::complex<double> getLowpassResponse(double f) const;
    std::complex<double> getBandpassResponse(double f) const;
    std::complex<double> getHighpassResponse(double f) const;
    std::complex<double> getNotchResponse(double f) const;

    // enum Section {
    //     LowpassSection,
    //     BandpassSection,
    //     HighpassSection,
    //     NotchSection,
    //     ///
    //     Section_Count,
    // };

private:
    float fLowpassGain;
    float fBandpassGain;
    float fHighpassGain;
    float fNotchGain;

    VAStateVariableFilter fLowpass[5];
    VAStateVariableFilter fBandpass[3];
    VAStateVariableFilter fHighpass[5];
    VAStateVariableFilter fNotch[5];
};
