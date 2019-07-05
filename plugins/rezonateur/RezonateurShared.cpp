#include "RezonateurShared.hpp"
#include <cmath>

void InitParameter(uint32_t index, Parameter &parameter)
{
    ParameterEnumerationValue *pev;

    switch (index) {
    case pIdBypass:
        parameter.designation = kParameterDesignationBypass;
        break;

    case pIdMode:
        parameter.symbol = "mode";
        parameter.name = "Mode";
        parameter.hints = kParameterIsInteger;
        parameter.ranges = ParameterRanges(0.0, 0.0, 3.0);
        pev = new ParameterEnumerationValue[4];
        parameter.enumValues.values = pev;
        parameter.enumValues.count = 4;
        parameter.enumValues.restrictedMode = true;
        pev[0] = ParameterEnumerationValue(0.0, "Low pass");
        pev[1] = ParameterEnumerationValue(1.0, "Band pass");
        pev[2] = ParameterEnumerationValue(2.0, "High pass");
        pev[3] = ParameterEnumerationValue(3.0, "Band pass - Notch");
        break;

    case pIdGain1:
        parameter.symbol = "gain1";
        parameter.name = "Low gain";
        parameter.hints = kParameterIsAutomable|kParameterIsLogarithmic;
        parameter.ranges = ParameterRanges(1.0 / std::sqrt(3.0 * M_SQRT1_2), 0.0, 1.0);
        break;
    case pIdCutoff1:
        parameter.symbol = "cutoff1";
        parameter.name = "Low cutoff";
        parameter.unit = "Hz";
        parameter.hints = kParameterIsAutomable;
        parameter.ranges = ParameterRanges(100.0, 60.0, 300.0);
        break;
    case pIdEmph1:
        parameter.symbol = "emph1";
        parameter.name = "Low emphasis";
        parameter.hints = kParameterIsAutomable;
        parameter.ranges = ParameterRanges(10.0, 1.0, 10.0);
        break;

    case pIdGain2:
        parameter.symbol = "gain2";
        parameter.name = "Mid gain";
        parameter.hints = kParameterIsAutomable|kParameterIsLogarithmic;
        parameter.ranges = ParameterRanges(1.0 / std::sqrt(3.0 * M_SQRT1_2), 0.0, 1.0);
        break;
    case pIdCutoff2:
        parameter.symbol = "cutoff2";
        parameter.name = "Mid cutoff";
        parameter.unit = "Hz";
        parameter.hints = kParameterIsAutomable;
        parameter.ranges = ParameterRanges(1000.0, 300.0, 1500.0);
        break;
    case pIdEmph2:
        parameter.symbol = "emph2";
        parameter.name = "Mid emphasis";
        parameter.hints = kParameterIsAutomable;
        parameter.ranges = ParameterRanges(10.0, 1.0, 10.0);
        break;

    case pIdGain3:
        parameter.symbol = "gain3";
        parameter.name = "High gain";
        parameter.hints = kParameterIsAutomable|kParameterIsLogarithmic;
        parameter.ranges = ParameterRanges(1.0 / std::sqrt(3.0 * M_SQRT1_2), 0.0, 1.0);
        break;
    case pIdCutoff3:
        parameter.symbol = "cutoff3";
        parameter.name = "High cutoff";
        parameter.unit = "Hz";
        parameter.hints = kParameterIsAutomable;
        parameter.ranges = ParameterRanges(5000.0, 1500.0, 7500.0);
        break;
    case pIdEmph3:
        parameter.symbol = "emph3";
        parameter.name = "High emphasis";
        parameter.hints = kParameterIsAutomable;
        parameter.ranges = ParameterRanges(10.0, 1.0, 10.0);
        break;
    case pIdDryGain:
        parameter.symbol = "dry";
        parameter.name = "Dry gain";
        parameter.hints = kParameterIsAutomable|kParameterIsLogarithmic;
        parameter.ranges = ParameterRanges(M_SQRT1_2, 0.0, 1.0);
        break;
    case pIdWetGain:
        parameter.symbol = "dry";
        parameter.name = "Dry gain";
        parameter.hints = kParameterIsAutomable|kParameterIsLogarithmic;
        parameter.ranges = ParameterRanges(M_SQRT1_2, 0.0, 1.0);
        break;

    default:
        DISTRHO_SAFE_ASSERT(false);
    }
}
