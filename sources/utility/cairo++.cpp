#include "cairo++.h"
#include <cmath>

cairo_surface_t *cairo_image_surface_create_from_png_data(const char *data, unsigned length)
{
    struct PngReaderData
    {
        const char *dataPtr;
        unsigned sizeLeft;
    };

    auto readSomePngData = [](void *closure,
                              unsigned char *data,
                              unsigned int length) noexcept -> cairo_status_t
    {
        PngReaderData &readerData = *reinterpret_cast<PngReaderData *>(closure);
        if (readerData.sizeLeft < length)
            return CAIRO_STATUS_READ_ERROR;

        memcpy(data, readerData.dataPtr, length);
        readerData.dataPtr += length;
        readerData.sizeLeft -= length;
        return CAIRO_STATUS_SUCCESS;
    };

    PngReaderData readerData;
    readerData.dataPtr = data;
    readerData.sizeLeft = length;
    return cairo_image_surface_create_from_png_stream(+readSomePngData, &readerData);
}

void cairo_set_source_rgba8(cairo_t *cr, ColorRGBA8 c)
{
    double k = 1.0 / 0xff;
    cairo_set_source_rgba(cr, c.r * k, c.g * k, c.b * k, c.a * k);
}

void cairo_snap_to_pixel(cairo_t *cr, double *x, double *y)
{
    (void)cr;
    *x = std::floor(*x) + 0.5;
    *y = std::floor(*y) + 0.5;
}

void cairo_move_to_pixel(cairo_t *cr, double x, double y)
{
    cairo_snap_to_pixel(cr, &x, &y);
    cairo_move_to(cr, x, y);
}

void cairo_line_to_pixel(cairo_t *cr, double x, double y)
{
    cairo_snap_to_pixel(cr, &x, &y);
    cairo_line_to(cr, x, y);
}
