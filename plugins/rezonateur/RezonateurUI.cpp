#include "RezonateurUI.hpp"

RezonateurUI::RezonateurUI()
    : UI(ui_width, ui_height)
{
}

RezonateurUI::~RezonateurUI()
{
}

const unsigned RezonateurUI::ui_width = 300;
const unsigned RezonateurUI::ui_height = 300;

void RezonateurUI::onDisplay()
{
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
