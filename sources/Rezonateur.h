#pragma once
#include "svf/VAStateVariableFilter.h"
#include "dsp/Oversampler.h"
#include <complex>
#include <memory>

class Rezonateur {
public:
    void init(double samplerate);

    void setFilterMode(int mode);
    void setFilterGain(unsigned nth, float gain);
    void setFilterCutoff(unsigned nth, float cutoff);
    void setFilterEmph(unsigned nth, float emph);
    int getFilterMode() const;
    float getFilterGain(unsigned nth) const;
    float getFilterCutoff(unsigned nth) const;
    float getFilterEmph(unsigned nth) const;

    unsigned getOversampling() const;
    void setOversampling(unsigned oversampling);

    void process(const float *input, float *output, unsigned count);

    double getResponseGain(double f) const;

    enum Mode {
        LowpassMode,
        BandpassMode,
        HighpassMode,
        BandpassNotchMode,
    };

private:
    template <class Oversampler> void processOversampled(Oversampler &oversampler, const float *input, float *output, unsigned count);
    template <class Oversampler> void processWithinBufferLimit(Oversampler &oversampler, const float *input, float *output, unsigned count);
    void getEffectiveFilterGains(float gains[3]) const;
    static int getFilterTypeForMode(int mode);

private:
    int fMode;
    float fFilterGains[3];
    float fFilterCutoffFreqs[3];
    float fFilterQ[3];
    VAStateVariableFilter fFilters[3];

    unsigned fOversampling;

    DSP::Oversampler<2, 32> fOversampler2x;
    DSP::Oversampler<4, 64> fOversampler4x;
    DSP::Oversampler<8, 64> fOversampler8x;
    enum { MaximumOversampling = 8 };

private:
    void allocateWorkBuffers(unsigned count);
    float *getWorkBuffer(unsigned index);

private:
    unsigned fNumWorkBuffers = 0;
    std::unique_ptr<float[]> fWorkBuffers;
    static constexpr unsigned sBufferLimit = 256;
};
