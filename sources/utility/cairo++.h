#pragma once
#include "utility/color.h"
#include <cairo/cairo.h>
#include <type_traits>
#include <memory>
#include <cstring>

cairo_surface_t *cairo_image_surface_create_from_png_data(const char *data, unsigned length);

void cairo_set_source_rgba8(cairo_t *cr, ColorRGBA8 c);

void cairo_snap_to_pixel(cairo_t *cr, double *x, double *y);
void cairo_move_to_pixel(cairo_t *cr, double x, double y);
void cairo_line_to_pixel(cairo_t *cr, double x, double y);

struct cairo_surface_deleter { void operator()(cairo_surface_t *x) const noexcept { cairo_surface_destroy(x); } };
typedef std::unique_ptr<std::remove_pointer<cairo_surface_t>::type, cairo_surface_deleter> cairo_surface_u;
