#pragma once
#include "Widget.hpp"
#include <functional>
class KnobSkin;

class SkinIndicator : public Widget {
public:
    SkinIndicator(const KnobSkin &skin, Widget *group);

    enum Orientation {
        Horizontal,
        Vertical,
    };

    double value() const noexcept { return fValue; }
    void setValue(double value);

    bool valueNotified() const { return fValueNotify; }
    void setValueNotified(bool notified);

    void setValueBounds(double v1, double v2);
    void setNumSteps(unsigned numSteps);

    void onDisplay() override;

    std::function<void(double)> ValueChangedCallback;

private:
    double clampToBounds(double value);

private:
    double fValue = 0;
    double fValueBound1 = 0, fValueBound2 = 1;
    bool fValueNotify = true;
    const KnobSkin &fSkin;
};
