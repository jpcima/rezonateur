#include "RezonateurUI.hpp"
#include "RezonateurPlugin.hpp"
#include "Artwork.hpp"
#include "Window.hpp"
#include "Cairo.hpp"
#include "components/ResponseView.hpp"
#include "components/SkinIndicator.hpp"
#include "components/SkinSlider.hpp"
#include "components/SkinToggleButton.hpp"
#include "utility/color.h"
#include <cmath>

#ifndef DISTRHO_UI_PATCH_INVERTED_BYPASS
    #pragma message("Please patch DPF with `resources/patch/DPF-bypass.patch`")
    #pragma message("The patch works around the value inversion issue (DISTRHO/DPF#150).")
#endif

RezonateurUI::RezonateurUI()
    : UI(ui_width, ui_height),
      fSkinBlackKnob(Artwork::polyknob_blackData, Artwork::polyknob_blackDataSize, 31),
      fSkinGreenKnob(Artwork::polyknob_greenData, Artwork::polyknob_greenDataSize, 31),
      fSkinYellowKnob(Artwork::polyknob_yellowData, Artwork::polyknob_yellowDataSize, 31),
      fSkinRedKnob(Artwork::polyknob_redData, Artwork::polyknob_redDataSize, 31),
      fSkinPowerSwitch(Artwork::power_switchData, Artwork::power_switchDataSize, 2),
      fSkinLevelMonitor(Artwork::level_monitorData, Artwork::level_monitorDataSize, 31),
      fSkinTextPassMode(Artwork::text_pass_modeData, Artwork::text_pass_modeDataSize, 1),
      fSkinTextBandKnobs(Artwork::text_band_knobsData, Artwork::text_band_knobsDataSize, 1),
      fSkinTextBandLow(Artwork::text_band_lowData, Artwork::text_band_lowDataSize, 1),
      fSkinTextBandMid(Artwork::text_band_midData, Artwork::text_band_midDataSize, 1),
      fSkinTextBandHigh(Artwork::text_band_highData, Artwork::text_band_highDataSize, 1)
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

    int xPassMode;
    int xBandKnobs[3];

    int xOffPassMode = -12;
    int xOffBandKnobs = -2;

    #warning XXX test
    int sx = 50;
    int sy = 380;
    xPassMode = sx;
    createSliderForParameter(fSkinBlackKnob, pIdMode, sx, sy);
    sx += 50;
    xBandKnobs[0] = sx;
    createSliderForParameter(fSkinGreenKnob, pIdCutoff1, sx, sy);
    sx += 30;
    createSliderForParameter(fSkinYellowKnob, pIdEmph1, sx, sy);
    sx += 30;
    createSliderForParameter(fSkinRedKnob, pIdGain1, sx, sy);
    sx += 50;
    xBandKnobs[1] = sx;
    createSliderForParameter(fSkinGreenKnob, pIdCutoff2, sx, sy);
    sx += 30;
    createSliderForParameter(fSkinYellowKnob, pIdEmph2, sx, sy);
    sx += 30;
    createSliderForParameter(fSkinRedKnob, pIdGain2, sx, sy);
    sx += 50;
    xBandKnobs[2] = sx;
    createSliderForParameter(fSkinGreenKnob, pIdCutoff3, sx, sy);
    sx += 30;
    createSliderForParameter(fSkinYellowKnob, pIdEmph3, sx, sy);
    sx += 30;
    createSliderForParameter(fSkinRedKnob, pIdGain3, sx, sy);
    sx += 50;
    createSliderForParameter(fSkinGreenKnob, pIdDryGain, sx, sy);
    sx += 30;
    createSliderForParameter(fSkinRedKnob, pIdWetGain, sx, sy);

    SkinIndicator *label;
    ///
    label = new SkinIndicator(fSkinTextPassMode, this);
    fMiscWidgets.push_back(std::unique_ptr<Widget>(label));
    label->setAbsolutePos(xPassMode + xOffPassMode, 360);
    ///
    label = new SkinIndicator(fSkinTextBandKnobs, this);
    fMiscWidgets.push_back(std::unique_ptr<Widget>(label));
    label->setAbsolutePos(xBandKnobs[0] + xOffBandKnobs, 360);
    label = new SkinIndicator(fSkinTextBandKnobs, this);
    fMiscWidgets.push_back(std::unique_ptr<Widget>(label));
    label->setAbsolutePos(xBandKnobs[1] + xOffBandKnobs, 360);
    label = new SkinIndicator(fSkinTextBandKnobs, this);
    fMiscWidgets.push_back(std::unique_ptr<Widget>(label));
    label->setAbsolutePos(xBandKnobs[2] + xOffBandKnobs, 360);
    ///
    label = new SkinIndicator(fSkinTextBandLow, this);
    fMiscWidgets.push_back(std::unique_ptr<Widget>(label));
    label->setAbsolutePos(xBandKnobs[0] + xOffBandKnobs, 480);
    label = new SkinIndicator(fSkinTextBandMid, this);
    fMiscWidgets.push_back(std::unique_ptr<Widget>(label));
    label->setAbsolutePos(xBandKnobs[1] + xOffBandKnobs, 480);
    label = new SkinIndicator(fSkinTextBandHigh, this);
    fMiscWidgets.push_back(std::unique_ptr<Widget>(label));
    label->setAbsolutePos(xBandKnobs[2] + xOffBandKnobs, 480);
    ///

    SkinIndicator *levelMonitor = new SkinIndicator(fSkinLevelMonitor, this);
    fLevelMonitor.reset(levelMonitor);
    levelMonitor->setAbsolutePos(380, 330);
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

#ifdef DISTRHO_UI_PATCH_INVERTED_BYPASS
    if (index == pIdBypass && hasInvertedBypass())
        value = 1.0f - value;
#endif

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

void RezonateurUI::uiIdle()
{
    RezonateurPlugin *dsp = (RezonateurPlugin *)getPluginInstancePointer();
    float level = dsp->getCurrentOutputLevel();
    fLevelMonitor->setValue(level);
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

    SkinSlider *sl = new SkinSlider(skin, this);
    fSliderForParameter[pid].reset(sl);
    sl->setAbsolutePos(x, y);
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

    SkinToggleButton *cb = new SkinToggleButton(skin, this);
    fToggleButtonForParameter[pid].reset(cb);
    cb->setAbsolutePos(x, y);

    Parameter param;
    InitParameter(pid, param);
    cb->setValue(param.ranges.def > 0.5f);

    cb->ValueChangedCallback =
        [this, pid](bool value) {
#ifdef DISTRHO_UI_PATCH_INVERTED_BYPASS
            if (pid == pIdBypass && hasInvertedBypass())
                value = 1.0f - value;
#endif
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
