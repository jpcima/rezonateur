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

void ResponseView::setColor(unsigned mode, ColorRGBA8 color)
{
    DISTRHO_SAFE_ASSERT_RETURN(mode < 4,);

    if (fColor[mode] == color)
        return;

    fColor[mode] = color;
    repaint();
}

void ResponseView::updateResponse()
{
    fCacheValid = false;
    repaint();
}

const double ResponseView::minFrequency = 10.0;
const double ResponseView::maxFrequency = 20000.0;
const double ResponseView::minGain = -40.0;
const double ResponseView::maxGain = +40.0;

static const double frequencyStops[] = {
    30.0, 100.0, 300.0, 1000.0, 3000.0, 10000.0
};
static const double magnitudeStops[] = {
    -30.0, -20.0, -10.0, +10.0, +20.0, +30.0
};

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

    Rezonateur &rez = fRez;
    const std::vector<double> &response = fResponse;

    ///
    cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1.0);
    const double dashes[] = { 1.0, 4.0 };
    cairo_set_dash(cr, dashes, sizeof(dashes) / sizeof(dashes[0]), 0);
    for (double f : frequencyStops) {
        double r = widthRatioOfFrequency(f);
        double x = r * (w - 1);
        cairo_move_to_pixel(cr, x, 0);
        cairo_line_to_pixel(cr, x, h);
        cairo_stroke(cr);
    }
    for (double m : magnitudeStops) {
        double r = (m - minGain) * (1.0 / (maxGain - minGain));
        double y = (1.0 - r) * (h - 1);
        cairo_move_to_pixel(cr, 0, y);
        cairo_line_to_pixel(cr, w, y);
        cairo_stroke(cr);
    }
    cairo_set_dash(cr, nullptr, 0, 0);
    {
        double m = 0.0;
        double r = (m - minGain) * (1.0 / (maxGain - minGain));
        double y = (1.0 - r) * (h - 1);
        cairo_move_to(cr, 0, y);
        cairo_line_to(cr, w, y);
        cairo_stroke(cr);
    }

    ///
    unsigned mode = rez.getFilterMode();
    cairo_set_line_width(cr, 2.0);
    cairo_set_source_rgba8(cr, fColor[mode]);

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
    unsigned size = getWidth();

    DISTRHO_SAFE_ASSERT_RETURN(size > 0,);

    fResponse.resize(size);
    double *response = fResponse.data();

    for (unsigned i = 0; i < size; ++i) {
        double r = i * (1.0 / (size - 1));
        double f = frequencyOfWidthRatio(r);
        response[i] = rez.getResponseGain(f);
    }
}

double ResponseView::frequencyOfWidthRatio(double r)
{
    return minFrequency * std::pow(maxFrequency / minFrequency, r);
}

double ResponseView::widthRatioOfFrequency(double f)
{
    return std::log(f * (1.0 / minFrequency)) * (1.0 / std::log(maxFrequency / minFrequency));
}
