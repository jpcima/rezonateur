/*
  ==============================================================================

    VAStateVariableFilter.cpp
    Created: 10 May 2015 2:41:43pm
    Author:  Jordan Harris

  ==============================================================================
*/

#include "VAStateVariableFilter.h"
#include <cmath>

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

float VAStateVariableFilter::processAudioSample(float input)
{
    // Filter processing:
    const float HP = (input - (2.0f * RCoeff + gCoeff) * z1_A - z2_A)
        / (1.0f + (2.0f * RCoeff * gCoeff) + gCoeff * gCoeff);

    const float BP = HP * gCoeff + z1_A;

    const float LP = BP * gCoeff + z2_A;

    const float UBP = 2.0f * RCoeff * BP;

    const float BShelf = input + UBP * KCoeff;

    const float Notch = input - UBP;

    const float AP = input - (4.0f * RCoeff * BP);

    const float Peak = LP - HP;

    z1_A = gCoeff * HP + BP;        // unit delay (state variable)
    z2_A = gCoeff * BP + LP;        // unit delay (state variable)

    // Selects which filter type this function will output.
    switch (filterType) {
    case SVFLowpass:
        return LP;
    case SVFBandpass:
        return BP;
    case SVFHighpass:
        return HP;
    case SVFUnitGainBandpass:
        return UBP;
    case SVFBandShelving:
        return BShelf;
    case SVFNotch:
        return Notch;
    case SVFAllpass:
        return AP;
    case SVFPeak:
        return Peak;
    default:
        return 0.0f;
    }
}

//==============================================================================
