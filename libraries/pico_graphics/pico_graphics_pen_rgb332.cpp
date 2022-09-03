#include "pico_graphics.hpp"
#include <string.h>

namespace pimoroni {
    PicoGraphics_PenRGB332::PicoGraphics_PenRGB332(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_RGB332;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
    }
    void PicoGraphics_PenRGB332::set_pen(uint c) {
        color = c;
    }
    void PicoGraphics_PenRGB332::set_pen(uint8_t r, uint8_t g, uint8_t b) {
        color = rgb_to_rgb332(r, g, b);
    }
    int PicoGraphics_PenRGB332::create_pen(uint8_t r, uint8_t g, uint8_t b) {
        return rgb_to_rgb332(r, g, b);
    }
    int PicoGraphics_PenRGB332::create_pen_hsv(float h, float s, float v) {
        return RGB::from_hsv(h, s, v).to_rgb332();
    }
    void PicoGraphics_PenRGB332::set_pixel(const Point &p) {
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf[p.y * bounds.w + p.x] = color;
    }
    void PicoGraphics_PenRGB332::set_pixel_span(const Point &p, uint l) {
        // pointer to byte in framebuffer that contains this pixel
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf = &buf[p.y * bounds.w + p.x];

        while(l--) {
            *buf++ = color;
        }
    }
    void PicoGraphics_PenRGB332::set_pixel_dither(const Point &p, const RGB &c) {
        if(!bounds.contains(p)) return;
        uint8_t _dmv = dither16_pattern[(p.x & 0b11) | ((p.y & 0b11) << 2)];

        uint8_t red = c.r & 0b11000000;        // Two bits red
        uint8_t red_r = c.r & 0b111111;        // Remaining six bits red
        red_r >>= 2;                           // Discard down to four bit

        uint8_t grn = (c.g & 0b11000000) >> 3; // Two bits green
        uint8_t grn_r = c.g & 0b111111;        // Remaining six bits green
        grn_r >>= 2;                           // Discard down to four bit

        uint8_t blu = (c.b & 0b10000000) >> 6; // One bit blue
        uint8_t blu_r = c.b & 0b1111111;       // Remaining seven bits green
        blu_