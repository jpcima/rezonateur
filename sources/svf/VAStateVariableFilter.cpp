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
    sampleRate = 44100.0;                // default sample rate when constructed
    filterType = SVFLowpass;            // lowpass filter by default

    gCoeff = 1.0;
    RCoeff = 1.0;
    KCoeff = 0.0;

    cutoffFreq = 1000.0;
    Q = resonanceToQ(0.5);
    shelfGain = 1.0;

    z1_A = 0.0;
    z2_A = 0.0;
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

void VAStateVariableFilter::setCutoffFreq(double newCutoffFreq)
{
    if (cutoffFreq == newCutoffFreq)
        return;

    cutoffFreq = newCutoffFreq;
    calcFilter();
}

void VAStateVariableFilter::setResonance(double newResonance)
{
    setQ(resonanceToQ(newResonance));
}

void VAStateVariableFilter::setQ(double newQ)
{
    if (Q == newQ)
        return;

    Q = newQ;
    calcFilter();
}

void VAStateVariableFilter::setShelfGain(double newGain)
{
    if (shelfGain == newGain)
        return;

    shelfGain = newGain;
    calcFilter();
}

void VAStateVariableFilter::setFilter(int newType, double newCutoffFreq,
                                      double newResonance, double newShelfGain)
{
    double newQ = resonanceToQ(newResonance);

    if (filterType == newType && cutoffFreq == newCutoffFreq && Q == newQ && shelfGain == newShelfGain)
        return;

    filterType = newType;
    cutoffFreq = newCutoffFreq;
    Q = newQ;
    shelfGain = newShelfGain;
    calcFilter();
}

void VAStateVariableFilter::setSampleRate(double newSampleRate)
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
    double wd = cutoffFreq * (2.0 * M_PI);
    double T = 1.0 / sampleRate;
    double wa = (2.0 / T) * std::tan(wd * T / 2.0);

    // Calculate g (gain element of integrator)
    gCoeff = wa * T / 2.0;            // Calculate g (gain element of integrator)

    // Calculate Zavalishin's R from Q (referred to as damping parameter)
    RCoeff = 1.0 / (2.0 * Q);

    // Gain for BandShelving filter
    KCoeff = shelfGain;
}

static double analogSaturate(double x)
{
    // simple filter analog saturation

    if (x > +1)
        x = 2. / 3.;
    else if (x < -1)
        x = -2. / 3.;
    else
        x = x - (x * x * x) * (1.0 / 3.0);

    return x;
}

template <int FilterType>
void VAStateVariableFilter::processInternally(float gain, const float *input, float *output, unsigned count)
{
    const double gCoeff = this->gCoeff;
    const double RCoeff = this->RCoeff;
    const double KCoeff = this->KCoeff;

    double z1_A = this->z1_A;
    double z2_A = this->z2_A;

    for (unsigned i = 0; i < count; ++i) {
        double in = gain * input[i];

        double HP = (in - ((2.0 * RCoeff + gCoeff) * z1_A) - z2_A)
            * (1.0 / (1.0 + (2.0 * RCoeff * gCoeff) + gCoeff * gCoeff));
        double BP = HP * gCoeff + z1_A;
        double LP = BP * gCoeff + z2_A;

        z1_A = analogSaturate(gCoeff * HP + BP);        // unit delay (state variable)
        z2_A = analogSaturate(gCoeff * BP + LP);        // unit delay (state variable)

        // Selects which filter type this function will output.
        double out = 0.0;
        if_constexpr (FilterType == SVFLowpass)
            out = LP;
        else if_constexpr (FilterType == SVFBandpass)
            out = BP;
        else if_constexpr (FilterType == SVFHighpass)
            out = HP;
        else if_constexpr (FilterType == SVFUnitGainBandpass)
            out = 2.0 * RCoeff * BP;
        else if_constexpr (FilterType == SVFBandShelving)
            out = in + 2.0 * RCoeff * KCoeff * BP;
        else if_constexpr (FilterType == SVFNotch)
            out = in - 2.0 * RCoeff * BP;
        else if_constexpr (FilterType == SVFAllpass)
            out = in - 4.0 * RCoeff * BP;
        else if_constexpr (FilterType == SVFPeak)
            out = LP - HP;

        output[i] = out;
    }

    this->z1_A = z1_A;
    this->z2_A = z2_A;
}

void VAStateVariableFilter::process(float gain, const float *input, float *output, unsigned count)
{
    switch (filterType) {
    case SVFLowpass:
        processInternally<SVFLowpass>(gain, input, output, count);
        break;
    case SVFBandpass:
        processInternally<SVFBandpass>(gain, input, output, count);
        break;
    case SVFHighpass:
        processInternally<SVFHighpass>(gain, input, output, count);
        break;
    case SVFUnitGainBandpass:
        processInternally<SVFUnitGainBandpass>(gain, input, output, count);
        break;
    case SVFBandShelving:
        processInternally<SVFBandShelving>(gain, input, output, count);
        break;
    case SVFNotch:
        processInternally<SVFNotch>(gain, input, output, count);
        break;
    case SVFAllpass:
        processInternally<SVFAllpass>(gain, input, output, count);
        break;
    case SVFPeak:
        processInternally<SVFPeak>(gain, input, output, count);
        break;
    default:
        for (unsigned i = 0; i < count; ++i)
            output[i] = gain * input[i];
    }
}

void VAStateVariableFilter::clear()
{
    z1_A = 0;
    z2_A = 0;
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
