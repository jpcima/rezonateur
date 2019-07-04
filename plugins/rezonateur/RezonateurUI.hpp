#pragma once
#include "DistrhoUI.hpp"
#include "RezonateurShared.hpp"
#include <cstdint>

class RezonateurUI : public UI {
public:
    RezonateurUI();
    ~RezonateurUI();

    static const unsigned ui_width;
    static const unsigned ui_height;

protected:
    void onDisplay() override;
    void parameterChanged(uint32_t index, float value) override;
};
