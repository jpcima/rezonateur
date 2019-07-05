#include "SkinSlider.hpp"
#include "KnobSkin.hpp"
#include "Window.hpp"
#include "Cairo.hpp"

SkinSlider::SkinSlider(const KnobSkin &skin, Widget *group)
    : Widget(group), fSkin(skin)
{
    setSize(skin.getWidth(), skin.getHeight());
}

void SkinSlider::setValue(double value)
{
    value = clampToBounds(value);

    if (fValue == value)
        return;

    fValue = value;
    if (ValueChangedCallback && fValueNotify)
        ValueChangedCallback(value);
    repaint();
}

void SkinSlider::setValueNotified(bool notified)
{
    fValueNotify = notified;
}

void SkinSlider::setValueBounds(double v1, double v2)
{
    fValueBound1 = v1;
    fValueBound2 = v2;
    setValue(fValue);
}

void SkinSlider::setNumSteps(unsigned numSteps)
{
    fNumSteps = numSteps;
}

void SkinSlider::setOrientation(Orientation ori)
{
    if (fOrientation == ori)
        return;

    fOrientation = ori;
    repaint();
}

bool SkinSlider::onMouse(const MouseEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    if (!fIsDragging && event.press && event.button == 1) {
        double fill = -1;

        if (fOrientation == Horizontal && mpos.getY() >= 0 && (unsigned)mpos.getY() < wsize.getHeight())
            fill = mpos.getX() / (double)wsize.getWidth();
        else if (fOrientation == Vertical && mpos.getX() >= 0 && (unsigned)mpos.getX() < wsize.getWidth())
            fill = 1.0 - (mpos.getY() / (double)wsize.getHeight());

        if (fill >= 0 && fill <= 1) {
            fIsDragging = true;
            setValue(fValueBound1 + fill * (fValueBound2 - fValueBound1));
            return true;
        }
    }
    else if (fIsDragging && !event.press && event.button == 1) {
        fIsDragging = false;
        return true;
    }

    return false;
}

bool SkinSlider::onMotion(const MotionEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    if (fIsDragging) {
        double fill;
        if (fOrientation == Horizontal)
            fill = mpos.getX() / (double)wsize.getWidth();
        else
            fill = 1.0 - (mpos.getY() / (double)wsize.getHeight());
        fill = (fill < 0) ? 0 : fill;
        fill = (fill > 1) ? 1 : fill;
        setValue(fValueBound1 + fill * (fValueBound2 - fValueBound1));
        return true;
    }

    return false;
}

bool SkinSlider::onScroll(const ScrollEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    bool inside =
        mpos.getX() >= 0 && mpos.getY() >= 0 &&
        (unsigned)mpos.getX() < wsize.getWidth() && (unsigned)mpos.getY() < wsize.getHeight();

    if (inside) {
        double amount = event.delta.getX() - event.delta.getY();
        if (fOrientation == Vertical)
            amount = -amount;
        setValue(fValue + amount * (fValueBound2 - fValueBound1) / fNumSteps);
        return true;
    }

    return false;
}

void SkinSlider::onDisplay()
{
    const KnobSkin &skin = fSkin;
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;

    int w = getWidth();
    int h = getHeight();

    //
    double v1 = fValueBound1;
    double v2 = fValueBound2;

    //
    double value = fValue;
    double fill = 0;
    if (v1 != v2)
        fill = (value - v1) / (v2 - v1);

    //
    cairo_surface_t *image = skin.getImageForRatio(fill);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_set_source_surface(cr, image, 0, 0);
    cairo_fill(cr);
}

double SkinSlider::clampToBounds(double value)
{
    double vmin = fValueBound1;
    double vmax = fValueBound2;
    if (vmin > vmax)
        std::swap(vmin, vmax);

    value = (value < vmin) ? vmin : value;
    value = (value > vmax) ? vmax : value;
    return value;
}
