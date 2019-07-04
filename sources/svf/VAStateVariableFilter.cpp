/*
  ==============================================================================

    VAStateVariableFilter.cpp
    Created: 10 May 2015 2:41:43pm
    Author:  Jordan Harris

  ==============================================================================
*/

#include "VAStateVariableFilter.h"
#include <cmath>

#if __cplusplus >= 201703L
# define if_constexpr if constexpr
#else
# define if_constexpr if
#endif

//==============================================================================

static double resonanceToQ(double resonance)
{
    return 1.0 / (2.0 * (1.0 - resonance));
}

//==============================================================================

VAStateVariableFilter::VAStateVariableFilter()
{
    sampleRate = 44100.0f;                // default sample rate when constructed
    filterType = SVFLowpass;            // lowpass filter by default

    gCoeff = 1.0f;
    RCoeff = 1.0f;
    KCoeff = 0.0f;

    cutoffFreq = 1000.0f;
    Q = static_cast<float>(resonanceToQ(0.5));
    shelfGain = 1.0f;

    z1_A = 0.0f;
    z2_A = 0.0f;
}

VAStateVariableFilter::~VAStateVariableFilter()
{
}

// Member functions for setting the filter's parameters (and sample rate).
//==============================================================================
void VAStateVariableFilter::setFilterType(int newType)
{
    filterType = newType;
}

void VAStateVariableFilter::setCutoffFreq(float newCutoffFreq)
{
    if (cutoffFreq == newCutoffFreq)
        return;

    cutoffFreq = newCutoffFreq;
    calcFilter();
}

void VAStateVariableFilter::setResonance(float newResonance)
{
    setQ(static_cast<float>(resonanceToQ(newResonance)));
}

void VAStateVariableFilter::setQ(float newQ)
{
    if (Q == newQ)
        return;

    Q = newQ;
    calcFilter();
}

void VAStateVariableFilter::setShelfGain(float newGain)
{
    if (shelfGain == newGain)
        return;

    shelfGain = newGain;
    calcFilter();
}

void VAStateVariableFilter::setFilter(int newType, float newCutoffFreq,
                                      float newResonance, float newShelfGain)
{
    float newQ = static_cast<float>(resonanceToQ(newResonance));

    if (filterType == newType && cutoffFreq == newCutoffFreq && Q == newQ && shelfGain == newShelfGain)
        return;

    filterType = newType;
    cutoffFreq = newCutoffFreq;
    Q = newQ;
    shelfGain = newShelfGain;
    calcFilter();
}

void VAStateVariableFilter::setSampleRate(float newSampleRate)
{
    if (sampleRate == newSampleRate)
        return;

    sampleRate = newSampleRate;
    calcFilter();
}

//==============================================================================
void VAStateVariableFilter::calcFilter()
{
    // prewarp the cutoff (for bilinear-transform filters)
    float wd = static_cast<float>(cutoffFreq * 2.0f * M_PI);
    float T = 1.0f / (float)sampleRate;
    float wa = (2.0f / T) * std::tan(wd * T / 2.0f);

    // Calculate g (gain element of integrator)
    gCoeff = wa * T / 2.0f;            // Calculate g (gain element of integrator)

    // Calculate Zavalishin's R from Q (referred to as damping parameter)
    RCoeff = 1.0f / (2.0f * Q);

    // Gain for BandShelving filter
    KCoeff = shelfGain;
}

template <int FilterType>
void VAStateVariableFilter::processInternally(const float *input, float *output, unsigned count)
{
    const float gCoeff = this->gCoeff;
    const float RCoeff = this->RCoeff;
    const float KCoeff = this->KCoeff;

    float z1_A = this->z1_A;
    float z2_A = this->z2_A;

    for (unsigned i = 0; i < count; ++i) {
        float in = input[i];

        float HP = (in - (2.0f * RCoeff + gCoeff) * z1_A - z2_A)
            * (1.0f / (1.0f + (2.0f * RCoeff * gCoeff) + gCoeff * gCoeff));
        float BP = HP * gCoeff + z1_A;
        float LP = BP * gCoeff + z2_A;

        z1_A = gCoeff * HP + BP;        // unit delay (state variable)
        z2_A = gCoeff * BP + LP;        // unit delay (state variable)

        // Selects which filter type this function will output.
        float out = 0.0f;
        if_constexpr (FilterType == SVFLowpass)
            out = LP;
        else if_constexpr (FilterType == SVFBandpass)
            out = BP;
        else if_constexpr (FilterType == SVFHighpass)
            out = HP;
        else if_constexpr (FilterType == SVFUnitGainBandpass)
            out = 2.0f * RCoeff * BP;
        else if_constexpr (FilterType == SVFBandShelving)
            out = in + 2.0f * RCoeff * KCoeff * BP;
        else if_constexpr (FilterType == SVFNotch)
            out = in - 2.0f * RCoeff * BP;
        else if_constexpr (FilterType == SVFAllpass)
            out = in - 4.0f * RCoeff * BP;
        else if_constexpr (FilterType == SVFPeak)
            out = LP - HP;

        output[i] = out;
    }

    this->z1_A = z1_A;
    this->z2_A = z2_A;
}

void VAStateVariableFilter::process(const float *input, float *output, unsigned count)
{
    switch (filterType) {
    case SVFLowpass:
        processInternally<SVFLowpass>(input, output, count);
        break;
    case SVFBandpass:
        processInternally<SVFBandpass>(input, output, count);
        break;
    case SVFHighpass:
        processInternally<SVFHighpass>(input, output, count);
        break;
    case SVFUnitGainBandpass:
        processInternally<SVFUnitGainBandpass>(input, output, count);
        break;
    case SVFBandShelving:
        processInternally<SVFBandShelving>(input, output, count);
        break;
    case SVFNotch:
        processInternally<SVFNotch>(input, output, count);
        break;
    case SVFAllpass:
        processInternally<SVFAllpass>(input, output, count);
        break;
    case SVFPeak:
        processInternally<SVFPeak>(input, output, count);
        break;
    default:
        for (unsigned i = 0; i < count; ++i)
            output[i] = input[i];
    }
}

std::complex<double> VAStateVariableFilter::calcTransfer(double freq) const
{
    double w = 2 * M_PI * freq;
    double wc = 2 * M_PI * cutoffFreq;

    switch (filterType) {
    case SVFLowpass:
        return calcTransferLowpass(w, wc, RCoeff);
    case SVFBandpass:
        return calcTransferBandpass(w, wc, RCoeff);
    case SVFHighpass:
        return calcTransferHighpass(w, wc, RCoeff);
    case SVFUnitGainBandpass:
        return calcTransferUnitGainBandpass(w, wc, RCoeff);
    case SVFBandShelving:
        return calcTransferBandShelving(w, wc, RCoeff, shelfGain);
    case SVFNotch:
        return calcTransferNotch(w, wc, RCoeff);
    case SVFAllpass:
        return calcTransferAllpass(w, wc, RCoeff);
    case SVFPeak:
        return calcTransferPeak(w, wc, RCoeff);
    default:
        return 0.0;
    }
}

//==============================================================================

std::complex<double> VAStateVariableFilter::calcTransferLowpass(double w, double wc, double r)
{
    std::complex<double> s = w * std::complex<double>(0, 1);
    return (wc * wc) / (s * s + 2.0 * r * wc * s + wc * wc);
}

std::complex<double> VAStateVariableFilter::calcTransferBandpass(double w, double wc, double r)
{
    std::complex<double> s = w * std::complex<double>(0, 1);
    return (wc * s) / (s * s + 2.0 * r * wc * s + wc * wc);
}

std::complex<double> VAStateVariableFilter::calcTransferHighpass(double w, double wc, double r)
{
    std::complex<double> s = w * std::complex<double>(0, 1);
    return (s * s) / (s * s + 2.0 * r * wc * s + wc * wc);
}

std::complex<double> VAStateVariableFilter::calcTransferUnitGainBandpass(double w, double wc, double r)
{
    return 2.0 * r * calcTransferBandpass(w, wc, r);
}

std::complex<double> VAStateVariableFilter::calcTransferBandShelving(double w, double wc, double r, double k)
{
    return 1.0 + k * calcTransferUnitGainBandpass(w, wc, r);
}

std::complex<double> VAStateVariableFilter::calcTransferNotch(double w, double wc, double r)
{
    return calcTransferBandShelving(w, wc, r, -1.0);
}

std::complex<double> VAStateVariableFilter::calcTransferAllpass(double w, double wc, double r)
{
    return calcTransferBandShelving(w, wc, r, -2.0);
}

std::complex<double> VAStateVariableFilter::calcTransferPeak(double w, double wc, double r)
{
    std::complex<double> s = w * std::complex<double>(0, 1);
    return (wc * wc - s * s) / (s * s + 2.0 * r * wc * s + wc * wc);
}

//==============================================================================
