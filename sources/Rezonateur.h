#pragma once
#include "svf/VAStateVariableFilter.h"
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

    void process(const float *input, float *output, unsigned count);

    double getResponseGain(double f) const;

    enum Mode {
        LowpassMode,
        BandpassMode,
        HighpassMode,
        BandpassNotchMode,
    };

private:
    void processWithinBufferLimit(const float *input, float *output, unsigned count);
    void getEffectiveFilterGains(float gains[3]) const;
    static int getFilterTypeForMode(int mode);

private:
    int fMode;
    float fFilterGains[3];
    VAStateVariableFilter fFilters[3];

private:
    void allocateWorkBuffers(unsigned count);
    float *getWorkBuffer(unsigned index);

private:
    unsigned fNumWorkBuffers = 0;
    std::unique_ptr<float[]> fWorkBuffers;
    static constexpr unsigned sBufferLimit = 256;
};
