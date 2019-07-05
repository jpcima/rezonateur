#pragma once
#include "DistrhoUI.hpp"
#include "RezonateurShared.hpp"
#include "Rezonateur.h"
#include "components/KnobSkin.hpp"
#include <memory>
#include <cstdint>
class ResponseView;
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

private:
    void updateParameterValue(uint32_t index, float value);
    void createSliderForParameter(const KnobSkin &skin, int pid, int x, int y);
    void createToggleButtonForParameter(const KnobSkin &skin, int pid, int x, int y);

private:
    std::unique_ptr<ResponseView> fResponseView;
    Rezonateur fRez;

    KnobSkin fSkinBlackKnob;
    KnobSkin fSkinGreenKnob;
    KnobSkin fSkinYellowKnob;
    KnobSkin fSkinRedKnob;
    KnobSkin fSkinPowerSwitch;

    std::unique_ptr<SkinSlider> fSliderForParameter[Parameter_Count];
    std::unique_ptr<SkinToggleButton> fToggleButtonForParameter[Parameter_Count];
};
