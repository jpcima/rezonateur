#pragma once
#include "DistrhoPlugin.hpp"
#include <cstdint>

void InitParameter(uint32_t index, Parameter &parameter);

enum {
    pIdBypass,

    pIdMode,

    pIdOversampling,

    pIdGain1,
    pIdCutoff1,
    pIdEmph1,

    pIdGain2,
    pIdCutoff2,
    pIdEmph2,

    pIdGain3,
    pIdCutoff3,
    pIdEmph3,

    pIdPreGain,
    pIdDryGain,
    pIdWetGain,

    ///
    Parameter_Count
};

enum {
    ///
    State_Count,
};
