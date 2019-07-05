#include "RezonateurUI.hpp"
#include "Window.hpp"
#include "Cairo.hpp"
#include "components/ResponseView.hpp"

RezonateurUI::RezonateurUI()
    : UI(ui_width, ui_height)
{
    double samplerate = getSampleRate();

    Rezonateur &rez = fRez;
    rez.init(samplerate);

    ResponseView *rezView = new ResponseView(rez, this);
    fResponseView.reset(rezView);

    rezView->setAbsolutePos(50, 20);
    rezView->setSize(400, 300);

    for (unsigned b = 0; b < 4; ++b) {
        static const uint8_t colors[4][4] = {
            {0xfc, 0xe9, 0x4f, 0xff},
            {0xfc, 0xaf, 0x3e, 0xff},
            {0x8a, 0xe2, 0x34, 0xff},
            {0x72, 0x9f, 0xcf, 0xff},
        };
        rezView->setColor(b, colors[b]);
    }
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
}

///
namespace DISTRHO {

UI *createUI()
{
    return new RezonateurUI;
}

} // namespace DISTRHO
