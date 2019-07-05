#include "RezonateurUI.hpp"
#include "Artwork.hpp"
#include "Window.hpp"
#include "Cairo.hpp"
#include "components/ResponseView.hpp"
#include "components/SkinSlider.hpp"
#include "components/SkinToggleButton.hpp"
#include "utility/color.h"
#include <cmath>

RezonateurUI::RezonateurUI()
    : UI(ui_width, ui_height),
      fSkinBlackKnob(Artwork::polyknob_blackData, Artwork::polyknob_blackDataSize, 31),
      fSkinGreenKnob(Artwork::polyknob_greenData, Artwork::polyknob_greenDataSize, 31),
      fSkinYellowKnob(Artwork::polyknob_yellowData, Artwork::polyknob_yellowDataSize, 31),
      fSkinRedKnob(Artwork::polyknob_redData, Artwork::polyknob_redDataSize, 31),
      fSkinPowerSwitch(Artwork::power_switchData, Artwork::power_switchDataSize, 2)
{
    double samplerate = getSampleRate();

    Rezonateur &rez = fRez;
    rez.init(samplerate);

    ResponseView *rezView = new ResponseView(rez, this);
    fResponseView.reset(rezView);

    rezView->setAbsolutePos(50, 20);
    rezView->setSize(400, 300);

    for (unsigned b = 0; b < 4; ++b) {
        static const ColorRGBA8 colors[4] = {
            {0xfc, 0xe9, 0x4f, 0xff},
            {0xfc, 0xaf, 0x3e, 0xff},
            {0x8a, 0xe2, 0x34, 0xff},
            {0x72, 0x9f, 0xcf, 0xff},
        };
        rezView->setColor(b, colors[b]);
    }

    createToggleButtonForParameter(fSkinPowerSwitch, pIdBypass, 50, 330);
    fToggleButtonForParameter[pIdBypass]->setHasInvertedAppearance(true);

    #warning XXX test
    int sx = 50;
    int sy = 350;
    createSliderForParameter(fSkinBlackKnob, pIdMode, sx, sy);
    sx += 50;
    createSliderForParameter(fSkinGreenKnob, pIdCutoff1, sx, sy);
    sx += 25;
    createSliderForParameter(fSkinYellowKnob, pIdEmph1, sx, sy);
    sx += 25;
    createSliderForParameter(fSkinRedKnob, pIdGain1, sx, sy);
    sx += 50;
    createSliderForParameter(fSkinGreenKnob, pIdCutoff2, sx, sy);
    sx += 25;
    createSliderForParameter(fSkinYellowKnob, pIdEmph2, sx, sy);
    sx += 25;
    createSliderForParameter(fSkinRedKnob, pIdGain2, sx, sy);
    sx += 50;
    createSliderForParameter(fSkinGreenKnob, pIdCutoff3, sx, sy);
    sx += 25;
    createSliderForParameter(fSkinYellowKnob, pIdEmph3, sx, sy);
    sx += 25;
    createSliderForParameter(fSkinRedKnob, pIdGain3, sx, sy);
    sx += 50;
    createSliderForParameter(fSkinGreenKnob, pIdWetGain, sx, sy);
    sx += 25;
    createSliderForParameter(fSkinRedKnob, pIdDryGain, sx, sy);
}

RezonateurUI::~RezonateurUI()
{
}

const unsigned RezonateurUI::ui_width = 500;
const unsigned RezonateurUI::ui_height = 500;

void RezonateurUI::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
    cairo_paint(cr);
}

void RezonateurUI::parameterChanged(uint32_t index, float value)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < Parameter_Count,);

    SkinSlider *sl = fSliderForParameter[index].get();
    if (sl) {
        sl->setValueNotified(false);
        sl->setValue(value);
        sl->setValueNotified(true);
    }

    SkinToggleButton *cb = fToggleButtonForParameter[index].get();
    if (cb) {
        cb->setValueNotified(false);
        cb->setValue(value);
        cb->setValueNotified(true);
    }

    updateParameterValue(index, value);
}

void RezonateurUI::updateParameterValue(uint32_t index, float value)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < Parameter_Count,);

    Rezonateur &rez = fRez;
    ResponseView &rezView = *fResponseView;

    switch (index) {
    case pIdMode:
        rez.setFilterMode((int)value);
        rezView.updateResponse();
        break;
    case pIdGain1:
        rez.setFilterGain(0, value);
        rezView.updateResponse();
        break;
    case pIdCutoff1:
        rez.setFilterCutoff(0, value);
        rezView.updateResponse();
        break;
    case pIdEmph1:
        rez.setFilterEmph(0, value);
        rezView.updateResponse();
        break;
    case pIdGain2:
        rez.setFilterGain(1, value);
        rezView.updateResponse();
        break;
    case pIdCutoff2:
        rez.setFilterCutoff(1, value);
        rezView.updateResponse();
        break;
    case pIdEmph2:
        rez.setFilterEmph(1, value);
        rezView.updateResponse();
        break;
    case pIdGain3:
        rez.setFilterGain(2, value);
        rezView.updateResponse();
        break;
    case pIdCutoff3:
        rez.setFilterCutoff(2, value);
        rezView.updateResponse();
        break;
    case pIdEmph3:
        rez.setFilterEmph(2, value);
        rezView.updateResponse();
        break;
    }
}

void RezonateurUI::createSliderForParameter(const KnobSkin &skin, int pid, int x, int y)
{
    DISTRHO_SAFE_ASSERT_RETURN(pid < Parameter_Count,);

    unsigned w = skin.getWidth();
    unsigned h = skin.getHeight();

    SkinSlider *sl = new SkinSlider(skin, this);
    fSliderForParameter[pid].reset(sl);
    sl->setAbsolutePos(x, y);
    sl->setSize(w, h);
    sl->setOrientation(SkinSlider::Vertical);

    Parameter param;
    InitParameter(pid, param);
    sl->setValueBounds(param.ranges.min, param.ranges.max);
    sl->setValue(param.ranges.def);

    int hints = param.hints;

    sl->ValueChangedCallback =
        [this, pid, hints](double value) {
            if (hints & kParameterIsInteger)
                value = std::lround(value);
            updateParameterValue(pid, value);
            setParameterValue(pid, value);
        };
}

void RezonateurUI::createToggleButtonForParameter(const KnobSkin &skin, int pid, int x, int y)
{
    DISTRHO_SAFE_ASSERT_RETURN(pid < Parameter_Count,);

    unsigned w = skin.getWidth();
    unsigned h = skin.getHeight();

    SkinToggleButton *cb = new SkinToggleButton(skin, this);
    fToggleButtonForParameter[pid].reset(cb);
    cb->setAbsolutePos(x, y);
    cb->setSize(w, h);

    Parameter param;
    InitParameter(pid, param);
    cb->setValue(param.ranges.def > 0.5f);

    cb->ValueChangedCallback =
        [this, pid](bool value) {
            updateParameterValue(pid, value);
            setParameterValue(pid, value);
        };
}

///
namespace DISTRHO {

UI *createUI()
{
    return new RezonateurUI;
}

} // namespace DISTRHO
