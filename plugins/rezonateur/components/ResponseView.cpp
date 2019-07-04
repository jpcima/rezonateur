#include "ResponseView.hpp"
#include "Rezonateur.h"
#include "Window.hpp"
#include "Cairo.hpp"
#include "utility/cairo++.h"
#include <cmath>
#include <cstring>

ResponseView::ResponseView(Rezonateur &rez, Widget *group)
    : Widget(group), fRez(rez)
{
    fResponse.reserve(1024);
}

void ResponseView::setBand(unsigned band)
{
    if (fBand == band)
        return;

    fBand = band;
    updateResponse();
}

void ResponseView::setColor(const uint8_t color[4])
{
    if (memcmp(fColor, color, 4) == 0)
        return;

    memcpy(fColor, color, 4);
    repaint();
}

void ResponseView::updateResponse()
{
    fCacheValid = false;
    repaint();
}

const double ResponseView::minFrequency = 10.0;
const double ResponseView::maxFrequency = 20000.0;
const double ResponseView::minGain = -80.0;
const double ResponseView::maxGain = +30.0;

void ResponseView::onDisplay()
{
    int w = getWidth();
    int h = getHeight();
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;

    if (w <= 0)
        return;

    recomputeResponseCache();

    cairo_save(cr);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_clip_preserve(cr);
    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 1.0);
    cairo_fill(cr);

    const std::vector<std::complex<double>> &response = fResponse;
    cairo_set_source_rgba8(cr, fColor[0], fColor[1], fColor[2], fColor[3]);

    bool havelasty = false;
    double lasty = 0;

    for (int x = 0; x < w; ++x) {
        double m = std::abs(response[x]);
        double g = 20.0 * std::log10(m);

        if (!std::isfinite(g))
            havelasty = false;
        else {
            double y = h * (1 - ((g - minGain) / (maxGain - minGain)));
            if (havelasty) {
                cairo_move_to(cr, x - 1, lasty);
                cairo_line_to(cr, x, y);
                cairo_stroke(cr);
            }
            havelasty = true;
            lasty = y;
        }
    }

    cairo_restore(cr);
}

void ResponseView::recomputeResponseCache()
{
    Rezonateur &rez = fRez;
    unsigned band = fBand;
    unsigned size = getWidth();

    DISTRHO_SAFE_ASSERT_RETURN(size > 0,);

    fResponse.resize(size);
    std::complex<double> *response = fResponse.data();

    for (unsigned i = 0; i < size; ++i) {
        double r = i * (1.0 / (size - 1));
        double f = std::exp(std::log(minFrequency) * (1 - r) + std::log(maxFrequency) * r);

        std::complex<double> h;
        switch (band) {
        case 0: h = rez.getLowpassResponse(f); break;
        case 1: h = rez.getBandpassResponse(f); break;
        case 2: h = rez.getHighpassResponse(f); break;
        case 3: h = rez.getNotchResponse(f); break;
        default: h = 0.0; DISTRHO_SAFE_ASSERT(false);
        }
        response[i] = h;
    }
}
