#pragma once
#include "../rezonateur/DistrhoPluginInfo.h"

#undef DISTRHO_PLUGIN_NAME
#undef DISTRHO_PLUGIN_URI
#undef DISTRHO_PLUGIN_UNIQUE_ID
#undef DISTRHO_PLUGIN_LABEL
#undef DISTRHO_PLUGIN_DESCRIPTION
#undef DISTRHO_PLUGIN_NUM_INPUTS
#undef DISTRHO_PLUGIN_NUM_OUTPUTS

#define DISTRHO_PLUGIN_NAME            u8"Rézonateur stereo"
#define DISTRHO_PLUGIN_URI             "http://jpcima.sdf1.org/lv2/rezonateur-stereo"
#define DISTRHO_PLUGIN_UNIQUE_ID       'r','e','z','S'
#define DISTRHO_PLUGIN_LABEL           u8"Rézonateur stereo"
#define DISTRHO_PLUGIN_DESCRIPTION     "3-band resonator stereo"
#define DISTRHO_PLUGIN_NUM_INPUTS      2
#define DISTRHO_PLUGIN_NUM_OUTPUTS     2
