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
    switch (index) {
    case pIdBypass:
        return fBypassed;
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
}

///
namespace DISTRHO {

Plugin *createPlugin()
{
    return new RezonateurPlugin;
}

} // namespace DISTRHO
