#pragma once
#include "DistrhoUI.hpp"
#include "RezonateurShared.hpp"
#include "Rezonateur.h"
#include "components/KnobSkin.hpp"
#include <list>
#include <memory>
#include <cstdint>
class ResponseView;
class SkinIndicator;
class SkinSlider;
class SkinToggleButton;

class RezonateurUI : public UI {
public:
    RezonateurUI();
    ~RezonateurUI();

    static const unsigned ui_width;
    static const unsigned ui_height;

protected:
    void onDisplay() override;
    void parameterChanged(uint32_t index, float value) override;
    void uiIdle() override;

private:
    void updateParameterValue(uint32_t index, float value);
    void createSliderForParameter(const KnobSkin &skin, int pid, int x, int y);
    void createToggleButtonForParameter(const KnobSkin &skin, int pid, int x, int y);

    double convertNormalizedToParameter(unsigned index, double value);
    double convertNormalizedFromParameter(unsigned index, double value);

private:
    std::unique_ptr<ResponseView> fResponseView;
    Rezonateur fRez;

    KnobSkin fSkinBlackKnob;
    KnobSkin fSkinGreenKnob;
    KnobSkin fSkinYellowKnob;
    KnobSkin fSkinRedKnob;
    KnobSkin fSkinPowerSwitch;
    KnobSkin fSkinLevelMonitor;

    KnobSkin fSkinTextPassMode;
    KnobSkin fSkinTextOversampling;
    KnobSkin fSkinTextSidePassMode;
    KnobSkin fSkinTextSideOversampling;
    KnobSkin fSkinTextBandKnobs;
    KnobSkin fSkinTextBandLow;
    KnobSkin fSkinTextBandMid;
    KnobSkin fSkinTextBandHigh;
    KnobSkin fSkinTextPreDryWet;

    std::unique_ptr<SkinIndicator> fLevelMonitor;
    std::list<std::unique_ptr<Widget>> fMiscWidgets;
    std::unique_ptr<SkinSlider> fSliderForParameter[Parameter_Count];
    std::unique_ptr<SkinToggleButton> fToggleButtonForParameter[Parameter_Count];

    Parameter fParameters[Parameter_Count];
};
