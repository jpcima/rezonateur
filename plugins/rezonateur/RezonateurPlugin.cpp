#include "RezonateurPlugin.hpp"
#include "DenormalDisabler.h"
#include <cstring>

constexpr unsigned RezonateurPlugin::NumChannels;

RezonateurPlugin::RezonateurPlugin()
    : Plugin(Parameter_Count, DISTRHO_PLUGIN_NUM_PROGRAMS, State_Count),
      fCurrentOutputLevel{}
{
    double samplerate = getSampleRate();

    for (unsigned c = 0; c < NumChannels; ++c) {
        fOutputLevelFollower[c].release(0.5 * samplerate);
        fRez[c].init(samplerate);
    }

    for (unsigned p = 0; p < Parameter_Count; ++p) {
        Parameter param;
        InitParameter(p, param);
        setParameterValue(p, param.ranges.def);
    }
}

RezonateurPlugin::~RezonateurPlugin()
{
}

const char *RezonateurPlugin::getLabel() const
{
    return DISTRHO_PLUGIN_LABEL;
}

const char *RezonateurPlugin::getMaker() const
{
    return DISTRHO_PLUGIN_MAKER;
}

const char *RezonateurPlugin::getLicense() const
{
    return DISTRHO_PLUGIN_LICENSE;
}

const char *RezonateurPlugin::getDescription() const
{
    return DISTRHO_PLUGIN_DESCRIPTION;
}

const char *RezonateurPlugin::getHomePage() const
{
    return DISTRHO_PLUGIN_HOMEPAGE;
}

uint32_t RezonateurPlugin::getVersion() const
{
    return d_version(DISTRHO_PLUGIN_VERSION);
}

int64_t RezonateurPlugin::getUniqueId() const
{
    return d_cconst(DISTRHO_PLUGIN_UNIQUE_ID);
}

void RezonateurPlugin::initParameter(uint32_t index, Parameter &parameter)
{
    InitParameter(index, parameter);
}

float RezonateurPlugin::getParameterValue(uint32_t index) const
{
    switch (index) {
    case pIdBypass:
        return fBypassed;
    case pIdMode:
        return fRez[0].getFilterMode();
    case pIdOversampling:
        return fRez[0].getOversampling();
    case pIdGain1:
        return fRez[0].getFilterGain(0);
    case pIdCutoff1:
        return fRez[0].getFilterCutoff(0);
    case pIdEmph1:
        return fRez[0].getFilterEmph(0);
    case pIdGain2:
        return fRez[0].getFilterGain(1);
    case pIdCutoff2:
        return fRez[0].getFilterCutoff(1);
    case pIdEmph2:
        return fRez[0].getFilterEmph(1);
    case pIdGain3:
        return fRez[0].getFilterGain(2);
    case pIdCutoff3:
        return fRez[0].getFilterCutoff(2);
    case pIdEmph3:
        return fRez[0].getFilterEmph(2);
    case pIdPreGain:
        return fPreGain;
    case pIdDryGain:
        return fDryGain;
    case pIdWetGain:
        return fWetGain;
    default:
        DISTRHO_SAFE_ASSERT_RETURN(false, 0);
    }
}

void RezonateurPlugin::setParameterValue(uint32_t index, float value)
{
    switch (index) {
    case pIdBypass:
        fBypassed = value > 0.5f;
        break;
    case pIdMode:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterMode((int)value);
        break;
    case pIdOversampling:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setOversampling((unsigned)value);
        break;
    case pIdGain1:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterGain(0, value);
        break;
    case pIdCutoff1:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterCutoff(0, value);
        break;
    case pIdEmph1:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterEmph(0, value);
        break;
    case pIdGain2:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterGain(1, value);
        break;
    case pIdCutoff2:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterCutoff(1, value);
        break;
    case pIdEmph2:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterEmph(1, value);
        break;
    case pIdGain3:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterGain(2, value);
        break;
    case pIdCutoff3:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterCutoff(2, value);
        break;
    case pIdEmph3:
        for (unsigned c = 0; c < NumChannels; ++c)
            fRez[c].setFilterEmph(2, value);
        break;
    case pIdPreGain:
        fPreGain = value;
        break;
    case pIdDryGain:
        fDryGain = value;
        break;
    case pIdWetGain:
        fWetGain = value;
        break;
    default:
        DISTRHO_SAFE_ASSERT_RETURN(false,);
    }
}

void RezonateurPlugin::run(const float **inputs, float **outputs, uint32_t frames)
{
    if (fBypassed) {
        for (unsigned c = 0; c < NumChannels; ++c) {
            memcpy(outputs[c], inputs[c], frames * sizeof(float));
            fOutputLevelFollower[c].clear();
            fCurrentOutputLevel[c] = 0;
        }
        return;
    }

    WebCore::DenormalDisabler noDenormals;

    float pre = fPreGain;
    float dry = fDryGain;
    float wet = fWetGain;

    for (unsigned c = 0; c < NumChannels; ++c) {
        Rezonateur &rez = fRez[c];
        const float *input = inputs[c];
        float *output = outputs[c];

        for (unsigned i = 0; i < frames; ++i)
            output[i] = pre * input[i];
        rez.process(output, output, frames);

        AmpFollower &levelFollower = fOutputLevelFollower[c];
        float level = fCurrentOutputLevel[c];

        for (unsigned i = 0; i < frames; ++i) {
            float out = dry * input[i] + wet * output[i];
            level = levelFollower.process(out);
            output[i] = out;
        }

        fCurrentOutputLevel[c] = level;
    }
}

float RezonateurPlugin::getCurrentOutputLevel() const
{
    float level = fCurrentOutputLevel[0];
    for (unsigned c = 1; c < NumChannels; ++c) {
        float other = fCurrentOutputLevel[c];
        level = (level > other) ? level : other;
    }
    return level;
}

///
namespace DISTRHO {

Plugin *createPlugin()
{
    return new RezonateurPlugin;
}

} // namespace DISTRHO
