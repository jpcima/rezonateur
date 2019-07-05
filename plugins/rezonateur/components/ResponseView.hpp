#pragma once
#include "Widget.hpp"
#include "utility/color.h"
#include <vector>
#include <cstdint>
class Rezonateur;

class ResponseView : public Widget {
public:
    explicit ResponseView(Rezonateur &rez, Widget *group);
    void setColor(unsigned mode, ColorRGBA8 color);
    void updateResponse();

    static const double minFrequency;
    static const double maxFrequency;
    static const double minGain;
    static const double maxGain;

protected:
    void onDisplay() override;

private:
    void recomputeResponseCache();

    static double frequencyOfWidthRatio(double r);
    static double widthRatioOfFrequency(double f);

private:
    Rezonateur &fRez;
    ColorRGBA8 fColor[4] = {};
    bool fCacheValid = false;
    std::vector<double> fResponse;
};
