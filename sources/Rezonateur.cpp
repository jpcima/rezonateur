#include "Rezonateur.h"
#include <cstring>
#include <cassert>

static constexpr unsigned bufferLimit = 256;

void Rezonateur::init(double samplerate)
{
    int mode = LowpassMode;
    int ftype = getFilterTypeForMode(mode);

    fMode = mode;

    for (unsigned i = 0; i < 3; ++i)
        fFilterGains[i] = 1.0;

    const double cutoffs[] = {300.0, 1800.0, 7600.0};
    const double q = 10.0;

    for (unsigned i = 0; i < 3; ++i) {
        VAStateVariableFilter &filter = fFilters[i];
        filter.setSampleRate(samplerate);
        filter.setFilterType(ftype);
        filter.setCutoffFreq(cutoffs[i]);
        filter.setQ(q);
    }
}

void Rezonateur::setFilterMode(int mode)
{
    int ftype = getFilterTypeForMode(mode);

    fMode = mode;

    for (unsigned i = 0; i < 3; ++i) {
        VAStateVariableFilter &filter = fFilters[i];
        filter.setFilterType(ftype);
        filter.clear();
    }
}

void Rezonateur::setFilterGain(unsigned nth, float gain)
{
    assert(nth < 3);
    fFilterGains[nth] = gain;
}

void Rezonateur::setFilterCutoff(unsigned nth, float cutoff)
{
    assert(nth < 3);
    fFilters[nth].setCutoffFreq(cutoff);
}

void Rezonateur::setFilterEmph(unsigned nth, float emph)
{
    assert(nth < 3);
    fFilters[nth].setQ(emph);
}

int Rezonateur::getFilterMode() const
{
    return fMode;
}

float Rezonateur::getFilterGain(unsigned nth) const
{
    assert(nth < 3);
    return fFilterGains[nth];
}

float Rezonateur::getFilterCutoff(unsigned nth) const
{
    assert(nth < 3);
    return fFilters[nth].getCutoffFreq();
}

float Rezonateur::getFilterEmph(unsigned nth) const
{
    assert(nth < 3);
    return fFilters[nth].getQ();
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
    float filterGains[3];
    getEffectiveFilterGains(filterGains);

    float accum[bufferLimit];

    ///
    for (unsigned b = 0; b < 3; ++b) {
        float g = filterGains[b];

        float filterOutput[bufferLimit];
        fFilters[b].process(input, filterOutput, count);

        if (b == 0) {
            for (unsigned i = 0; i < count; ++i)
                accum[i] = g * filterOutput[i];
        }
        else {
            for (unsigned i = 0; i < count; ++i)
                accum[i] += g * filterOutput[i];
        }
    }

    memcpy(output, accum, count * sizeof(float));
}

void Rezonateur::getEffectiveFilterGains(float gains[3]) const
{
    // must invert the middle filter in bandpass mode
    gains[0] = fFilterGains[0];
    gains[1] = (fMode != BandpassMode) ? fFilterGains[1] : -fFilterGains[1];
    gains[2] = fFilterGains[2];
}

int Rezonateur::getFilterTypeForMode(int mode)
{
    switch (mode) {
    default:
        assert(false);
        /* fall through */
    case LowpassMode:
        return SVFLowpass;
    case BandpassMode:
    case BandpassNotchMode:
        return SVFBandpass;
    case HighpassMode:
        return SVFHighpass;
    }
}

double Rezonateur::getResponseGain(double f) const
{
    std::complex<double> h = 0;
    float filterGains[3];
    getEffectiveFilterGains(filterGains);

    for (unsigned i = 0; i < 3; ++i) {
        double g = filterGains[i];
        h += g * fFilters[i].calcTransfer(f);
    }

    return std::abs(h);
}
