#include <cairo/cairo.h>
#include <cstdint>

inline void cairo_set_source_rgba8(cairo_t *cr, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    double k = 1.0 / 0xff;
    cairo_set_source_rgba(cr, r * k, g * k, b * k, a * k);
}

