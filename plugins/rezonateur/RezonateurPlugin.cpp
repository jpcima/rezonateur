#include "RezonateurPlugin.hpp"
#include "DenormalDisabler.h"
#include <cstring>

RezonateurPlugin::RezonateurPlugin()
    : Plugin(Parameter_Count, DISTRHO_PLUGIN_NUM_PROGRAMS, State_Count)
{
    fRez.init(getSampleRate());

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
    const Rezonateur &rez = fRez;

    switch (index) {
    case pIdBypass:
        return fBypassed;
    case pIdMode:
        return rez.getFilterMode();
    case pIdGain1:
        return rez.getFilterGain(0);
    case pIdCutoff1:
        return rez.getFilterCutoff(0);
    case pIdEmph1:
        return rez.getFilterEmph(0);
    case pIdGain2:
        return rez.getFilterGain(1);
    case pIdCutoff2:
        return rez.getFilterCutoff(1);
    case pIdEmph2:
        return rez.getFilterEmph(1);
    case pIdGain3:
        return rez.getFilterGain(2);
    case pIdCutoff3:
        return rez.getFilterCutoff(2);
    case pIdEmph3:
        return rez.getFilterEmph(2);
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
    Rezonateur &rez = fRez;

    switch (index) {
    case pIdBypass:
        fBypassed = value > 0.5f;
        break;
    case pIdMode:
        rez.setFilterMode((int)value);
        break;
    case pIdGain1:
        rez.setFilterGain(0, value);
        break;
    case pIdCutoff1:
        rez.setFilterCutoff(0, value);
        break;
    case pIdEmph1:
        rez.setFilterEmph(0, value);
        break;
    case pIdGain2:
        rez.setFilterGain(1, value);
        break;
    case pIdCutoff2:
        rez.setFilterCutoff(1, value);
        break;
    case pIdEmph2:
        rez.setFilterEmph(1, value);
        break;
    case pIdGain3:
        rez.setFilterGain(2, value);
        break;
    case pIdCutoff3:
        rez.setFilterCutoff(2, value);
        break;
    case pIdEmph3:
        rez.setFilterEmph(2, value);
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
    const float *input = inputs[0];
    float *output = outputs[0];

    if (fBypassed) {
        memcpy(output, input, frames * sizeof(float));
        return;
    }

    WebCore::DenormalDisabler noDenormals;

    Rezonateur &rez = fRez;
    rez.process(input, output, frames);

    float dry = fDryGain;
    float wet = fWetGain;

    for (unsigned i = 0; i < frames; ++i)
        output[i] = dry * input[i] + wet * output[i];
}

///
namespace DISTRHO {

Plugin *createPlugin()
{
    return new RezonateurPlugin;
}

} // namespace DISTRHO
