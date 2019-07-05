#pragma once
#include "Widget.hpp"
#include <vector>
#include <cstdint>
class Rezonateur;

class ResponseView : public Widget {
public:
    explicit ResponseView(Rezonateur &rez, Widget *group);
    void setColor(unsigned mode, const uint8_t color[3]);
    void updateResponse();

    static const double minFrequency;
    static const double maxFrequency;
    static const double minGain;
    static const double maxGain;

protected:
    void onDisplay() override;

private:
    void recomputeResponseCache();

private:
    Rezonateur &fRez;
    uint8_t fColor[4][4] = {};
    bool fCacheValid = false;
    std::vector<double> fResponse;
};
