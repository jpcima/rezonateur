#pragma once
#include "DistrhoPlugin.hpp"
#include "RezonateurShared.hpp"
#include "Rezonateur.h"
#include "dsp/AmpFollower.hpp"
#include <cstdint>

class RezonateurPlugin : public Plugin {
    static constexpr unsigned NumChannels = DISTRHO_PLUGIN_NUM_INPUTS;

    static_assert(DISTRHO_PLUGIN_NUM_INPUTS == DISTRHO_PLUGIN_NUM_OUTPUTS,
                  "Need to have as many inputs as outputs");

public:
    RezonateurPlugin();
    ~RezonateurPlugin();

    const char *getLabel() const override;
    const char *getMaker() const override;
    const char *getLicense() const override;
    const char *getDescription() const override;
    const char *getHomePage() const override;
    uint32_t getVersion() const override;
    int64_t getUniqueId() const override;

    void initParameter(uint32_t index, Parameter &parameter) override;
    float getParameterValue(uint32_t index) const override;
    void setParameterValue(uint32_t index, float value) override;

    void run(const float **inputs, float **outputs, uint32_t frames) override;

    float getCurrentOutputLevel() const;

private:
    bool fBypassed;
    float fPreGain;
    float fDryGain;
    float fWetGain;
    float fCurrentOutputLevel[NumChannels];
    AmpFollower fOutputLevelFollower[NumChannels];
    Rezonateur fRez[NumChannels];
};
