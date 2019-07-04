#pragma once
#include "DistrhoUI.hpp"
#include "RezonateurShared.hpp"
#include "Rezonateur.h"
#include <memory>
#include <cstdint>
class ResponseView;

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
    std::unique_ptr<ResponseView> fResponseView[4];
    Rezonateur fRez;
};
