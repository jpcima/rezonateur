#include "Rezonateur.h"
#include <cstring>
#include <cassert>

void Rezonateur::init(double samplerate)
{
    allocateWorkBuffers(3 * MaximumOversampling);

    int mode = LowpassMode;
    int ftype = getFilterTypeForMode(mode);

    fMode = mode;
    fOversampling = 1;

    for (unsigned i = 0; i < 3; ++i)
        fFilterGains[i] = 1.0;

    const double cutoffs[] = {300.0, 1800.0, 7600.0};
    const double q = 10.0;

    for (unsigned i = 0; i < 3; ++i) {
        VAStateVariableFilter &filter = fFilters[i];
        filter.setSampleRate(samplerate);
        filter.setFilterType(ftype);
        filter.setCutoffFreq(fFilterCutoffFreqs[i] = cutoffs[i]);
        filter.setQ(fFilterQ[i] = q);
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
    fFilters[nth].setCutoffFreq((fFilterCutoffFreqs[nth] = cutoff) / fOversampling);
}

void Rezonateur::setFilterEmph(unsigned nth, float emph)
{
    assert(nth < 3);
    fFilters[nth].setQ(fFilterQ[nth] = emph);
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
    return fFilterCutoffFreqs[nth];
}

float Rezonateur::getFilterEmph(unsigned nth) const
{
    assert(nth < 3);
    return fFilterQ[nth];
}

unsigned Rezonateur::getOversampling() const
{
    return fOversampling;
}

void Rezonateur::setOversampling(unsigned oversampling)
{
    switch (oversampling) {
    default:
        assert(false);
        /* fall through */
    case 1:
        oversampling = 1;
        if (fOversampling == oversampling)
            return;
        break;
    case 2:
        oversampling = 2;
        if (fOversampling == oversampling)
            return;
        fOversampler2x.reset();
        break;
    case 4:
        oversampling = 4;
        if (fOversampling == oversampling)
            return;
        fOversampler4x.reset();
        break;
    case 8:
        oversampling = 8;
        if (fOversampling == oversampling)
            return;
        fOversampler8x.reset();
        break;
    }

    fOversampling = oversampling;

    for (unsigned b = 0; b < 3; ++b) {
        VAStateVariableFilter &filter = fFilters[b];
        filter.setCutoffFreq(fFilterCutoffFreqs[b] / oversampling);
        filter.clear();
    }
}

void Rezonateur::process(const float *input, float *output, unsigned count)
{
    switch (fOversampling) {
    default:
        assert(false);
        /* fall through */
    case 1:
        DSP::NoOversampler noOversampler;
        processOversampled(noOversampler, input, output, count);
        break;
    case 2:
        processOversampled(fOversampler2x, input, output, count);
        break;
    case 4:
        processOversampled(fOversampler4x, input, output, count);
        break;
    case 8:
        processOversampled(fOversampler8x, input, output, count);
        break;
    }
}

template <class Oversampler> void Rezonateur::processOversampled(Oversampler &oversampler, const float *input, float *output, unsigned count)
{
    while (count > 0) {
        unsigned current = (count < sBufferLimit) ? count : sBufferLimit;
        processWithinBufferLimit(oversampler, input, output, current);
        input += current;
        output += current;
        count -= current;
    }
}

template <class Oversampler> void Rezonateur::processWithinBufferLimit(Oversampler &oversampler, const float *input, float *output, unsigned count)
{
    constexpr unsigned ratio = Oversampler::Ratio;

    float filterGains[3];
    getEffectiveFilterGains(filterGains);

    float *accum = getWorkBuffer(0 * MaximumOversampling);
    float *filterOutput = getWorkBuffer(1 * MaximumOversampling);

    ///
    if (ratio > 1) {
        float *filterInput = getWorkBuffer(2 * MaximumOversampling);
        for (unsigned i = 0; i < count; ++i) {
            filterInput[i * ratio] = oversampler.upsample(input[i]);
            for (unsigned o = 1; o < ratio; ++o)
                filterInput[i * ratio + o] = oversampler.uppad(o);
        }
        input = filterInput;
    }

    ///
    fFilters[0].process(filterGains[0], input, accum, count * ratio);
    for (unsigned b = 1; b < 3; ++b) {
        fFilters[b].process(filterGains[b], input, filterOutput, count * ratio);
        for (unsigned i = 0; i < count * ratio; ++i)
            accum[i] += filterOutput[i];
    }

    ///
    for (unsigned i = 0; i < count; ++i) {
        output[i] = oversampler.downsample(accum[i * ratio]);
        for (unsigned o = 1; o < ratio; ++o)
            oversampler.downstore(accum[i * ratio + o]);
    }
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

void Rezonateur::allocateWorkBuffers(unsigned count)
{
    fWorkBuffers.reset(new float[count * sBufferLimit]);
    fNumWorkBuffers = count;
}

float *Rezonateur::getWorkBuffer(unsigned index)
{
    assert(index < fNumWorkBuffers);
    return &fWorkBuffers[index * sBufferLimit];
}

constexpr unsigned Rezonateur::sBufferLimit;
