#pragma once
#include <cstdint>

#define DISTRHO_PLUGIN_BRAND           "Jean Pierre Cimalando"
#define DISTRHO_PLUGIN_NAME            "Rezonateur"
#define DISTRHO_PLUGIN_URI             "http://jpcima.sdf1.org/lv2/rezonateur"
#define DISTRHO_PLUGIN_HOMEPAGE        "https://github.com/jpcima/rezonateur"
#define DISTRHO_PLUGIN_UNIQUE_ID       'r','e','z','o'
#define DISTRHO_PLUGIN_VERSION         0,0,0
#define DISTRHO_PLUGIN_LABEL           "Rezonateur"
#define DISTRHO_PLUGIN_LICENSE         "http://spdx.org/licenses/BSL-1.0"
#define DISTRHO_PLUGIN_MAKER           "Jean Pierre Cimalando"
#define DISTRHO_PLUGIN_DESCRIPTION     "4-band resonator"
#define DISTRHO_PLUGIN_NUM_INPUTS      1
#define DISTRHO_PLUGIN_NUM_OUTPUTS     1
#define DISTRHO_PLUGIN_IS_SYNTH        0
#define DISTRHO_PLUGIN_HAS_UI          1
#define DISTRHO_PLUGIN_HAS_EMBED_UI    1
#define DISTRHO_PLUGIN_HAS_EXTERNAL_UI 0
#define DISTRHO_PLUGIN_IS_RT_SAFE      1
#define DISTRHO_PLUGIN_WANT_PROGRAMS   0
#define DISTRHO_PLUGIN_WANT_STATE      0
#define DISTRHO_PLUGIN_WANT_FULL_STATE 0
#define DISTRHO_PLUGIN_NUM_PROGRAMS    0

// for level monitoring
#define DISTRHO_PLUGIN_WANT_DIRECT_ACCESS 1

enum {
    pIdBypass,

    pIdMode,

    pIdGain1,
    pIdCutoff1,
    pIdEmph1,

    pIdGain2,
    pIdCutoff2,
    pIdEmph2,

    pIdGain3,
    pIdCutoff3,
    pIdEmph3,

    pIdDryGain,
    pIdWetGain,

    ///
    Parameter_Count
};

enum {
    ///
    State_Count,
};
