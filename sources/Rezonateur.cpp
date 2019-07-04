#include "Rezonateur.h"

static constexpr unsigned bufferLimit = 256;

void Rezonateur::init(double samplerate)
{
    fLowpassGain = 1.0;
    fBandpassGain = 1.0;
    fHighpassGain = 1.0;
    fNotchGain = 1.0;

    struct FilterDefaults {
        int type;
        double cutoff;
        double q;
    };

    const FilterDefaults lowpassDefaults[] = {
        {SVFPeak, 300.0, 10.0},
        {SVFPeak, 1800.0, 10.0},
        {SVFLowpass, 7600.0, 10.0},
        {SVFNotch, 400.0, 1.0},
        {SVFNotch, 2100.0, 1.0},
    };
    const FilterDefaults bandpassDefaults[] = {
        {SVFHighpass, 300.0, 10.0},
        {SVFPeak, 1800.0, 10.0},
        {SVFLowpass, 7600.0, 10.0},
    };
    const FilterDefaults highpassDefaults[] = {
        {SVFHighpass, 300.0, 10.0},
        {SVFPeak, 1800.0, 10.0},
        {SVFPeak, 7600.0, 10.0},
        {SVFNotch, 1000.0, 1.0},
        {SVFNotch, 6100.0, 1.0},
    };
    const FilterDefaults notchDefaults[] = {
        {SVFHighpass, 300.0, 10.0},
        {SVFPeak, 1800.0, 10.0},
        {SVFLowpass, 7600.0, 10.0},
        {SVFNotch, 700.0, 1.0},
        {SVFNotch, 3700.0, 1.0},
    };

    for (unsigned i = 0; i < 5; ++i) {
        VAStateVariableFilter &filter = fLowpass[i];
        const FilterDefaults &def = lowpassDefaults[i];
        filter.setSampleRate(samplerate);
        filter.setFilterType(def.type);
        filter.setCutoffFreq(def.cutoff);
        filter.setQ(def.q);
    }

    for (unsigned i = 0; i < 3; ++i) {
        VAStateVariableFilter &filter = fBandpass[i];
        const FilterDefaults &def = bandpassDefaults[i];
        filter.setSampleRate(samplerate);
        filter.setFilterType(def.type);
        filter.setCutoffFreq(def.cutoff);
        filter.setQ(def.q);
    }

    for (unsigned i = 0; i < 5; ++i) {
        VAStateVariableFilter &filter = fHighpass[i];
        const FilterDefaults &def = highpassDefaults[i];
        filter.setSampleRate(samplerate);
        filter.setFilterType(def.type);
        filter.setCutoffFreq(def.cutoff);
        filter.setQ(def.q);
    }

    for (unsigned i = 0; i < 5; ++i) {
        VAStateVariableFilter &filter = fNotch[i];
        const FilterDefaults &def = notchDefaults[i];
        filter.setSampleRate(samplerate);
        filter.setFilterType(def.type);
        filter.setCutoffFreq(def.cutoff);
        filter.setQ(def.q);
    }
}

void Rezonateur::process(const float *input, float *output, unsigned count)
{
    while (count > 0) {
        unsigned current = (count < bufferLimit) ? count : bufferLimit;
        processWithinBufferLimit(input, output, count);
        input += current;
        output += current;
        count -= current;
    }
}

void Rezonateur::processWithinBufferLimit(const float *input, float *output, unsigned count)
{
    float lowpassGain = fLowpassGain;
    float bandpassGain = fBandpassGain;
    float highpassGain = fHighpassGain;
    float notchGain = fNotchGain;

    float lowpass[bufferLimit];
    float bandpass[bufferLimit];
    float highpass[bufferLimit];
    float notch[bufferLimit];

    for (unsigned i = 0; i < count; ++i)
        lowpass[i] = input[i] * lowpassGain;
    for (unsigned i = 0; i < 5; ++i)
        fLowpass[i].process(lowpass, lowpass, count);

    for (unsigned i = 0; i < count; ++i)
        bandpass[i] = input[i] * bandpassGain;
    for (unsigned i = 0; i < 3; ++i)
        fBandpass[i].process(bandpass, bandpass, count);

    for (unsigned i = 0; i < count; ++i)
        highpass[i] = input[i] * highpassGain;
    for (unsigned i = 0; i < 5; ++i)
        fHighpass[i].process(highpass, highpass, count);

    for (unsigned i = 0; i < count; ++i)
        notch[i] = input[i] * notchGain;
    for (unsigned i = 0; i < 5; ++i)
        fNotch[i].process(notch, notch, count);

    for (unsigned i = 0; i < count; ++i)
        output[i] = lowpass[i] + bandpass[i] + highpass[i] + notch[i];
}

std::complex<double> Rezonateur::getLowpassResponse(double f) const
{
    std::complex<double> r(fLowpassGain, 0);
    for (unsigned i = 0; i < 5; ++i) {
        std::complex<double> h = fLowpass[i].calcTransfer(f);
        r *= h;
    }
    return r;
}

std::complex<double> Rezonateur::getBandpassResponse(double f) const
{
    std::complex<double> r(fBandpassGain, 0);
    for (unsigned i = 0; i < 3; ++i) {
        std::complex<double> h = fBandpass[i].calcTransfer(f);
        r *= h;
    }
    return r;
}

std::complex<double> Rezonateur::getHighpassResponse(double f) const
{
    std::complex<double> r(fHighpassGain, 0);
    for (unsigned i = 0; i < 5; ++i) {
        std::complex<double> h = fHighpass[i].calcTransfer(f);
        r *= h;
    }
    return r;
}

std::complex<double> Rezonateur::getNotchResponse(double f) const
{
    std::complex<double> r(fNotchGain, 0);
    for (unsigned i = 0; i < 5; ++i) {
        std::complex<double> h = fNotch[i].calcTransfer(f);
        r *= h;
    }
    return r;
}
