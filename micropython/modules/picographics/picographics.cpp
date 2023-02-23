#include "drivers/st7789/st7789.hpp"
#include "drivers/st7735/st7735.hpp"
#include "drivers/sh1107/sh1107.hpp"
#include "drivers/uc8151/uc8151.hpp"
#include "drivers/uc8159/uc8159.hpp"
#include "drivers/st7567/st7567.hpp"
#include "drivers/inky73/inky73.hpp"
#include "drivers/psram_display/psram_display.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "common/pimoroni_i2c.hpp"

#include "micropython/modules/util.hpp"

using namespace pimoroni;

extern "C" {
#include "picographics.h"
#include "micropython/modules/pimoroni_bus/pimoroni_bus.h"
#include "pimoroni_i2c.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"

std::string mp_obj_to_string_r(const mp_obj_t &obj) {
    if(mp_obj_is_str_or_bytes(obj)) {
        GET_STR_DATA_LEN(obj, str, str_len);
        return (const char*)str;
    }
    mp_raise_TypeError("can't convert object to str implicitly");
}

typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *spritedata;
    void *buffer;
    _PimoroniI2C_obj_t *i2c;
    //mp_obj_t scanline_callback; // Not really feasible in MicroPython
} ModPicoGraphics_obj_t;

bool get_display_settings(PicoGraphicsDisplay display, int &width, int &height, int &rotate, int &pen_type, PicoGraphicsBusType &bus_type) {
    switch(display) {
        case DISPLAY_PICO_DISPLAY:
            width = 240;
            height = 135;
            bus_type = BUS_SPI;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_270;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_TUFTY_2040:
            width = 320;
            height = 240;
            bus_type = BUS_PARALLEL;
            // Tufty display is upside-down
            if(rotate == -1) rotate = (int)Rotation::ROTATE_180;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_PICO_DISPLAY_2:
            width = 320;
            height = 240;
            bus_type = BUS_SPI;
            // Tufty display is upside-down
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_PICO_EXPLORER:
        case DISPLAY_LCD_240X240:
        case DISPLAY_ENVIRO_PLUS:
            width = 240;
            height = 240;
            bus_type = BUS_SPI;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_ROUND_LCD_240X240:
            width = 240;
            height = 240;
            bus_type = BUS_SPI;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_LCD_160X80:
            width = 160;
            height = 80;
            bus_type = BUS_SPI;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_I2C_OLED_128X128:
            width = 128;
            height = 128;
            bus_type = BUS_I2C;
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_1BIT;
            break;
        case DISPLAY_INKY_PACK:
            width = 296;
            height = 128;
            bus_type = BUS_SPI;
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_1BIT;
            break;
        case DISPLAY_INKY_FRAME:
            width = 600;
            height = 448;
            bus_type = BUS_SPI;
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_P4;
            break;
        case DISPLAY_INKY_FRAME_4:
            width = 640;
            height = 400;
            bus_type = BUS_SPI;
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_P4;
            break;
        case DISPLAY_GALACTIC_UNICORN:
            width = 53;
            height = 11;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        case DISPLAY_GFX_PACK:
            width = 128;
            height = 64;
            bus_type = BUS_SPI;
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_1BIT;
            break;
        case DISPLAY_INTERSTATE75_32X32:
            width = 32;
            height = 32;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        case DISPLAY_INTERSTATE75_64X32:
            width = 64;
            height = 32;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        case DISPLAY_INTERSTATE75_96X32:
            width = 96;
            height = 32;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        case DISPLAY_INTERSTATE75_128X32:
            width = 128;
            height = 32;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        case DISPLAY_INTERSTATE75_64X64:
            width = 64;
            height = 64;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        case DISPLAY_INTERSTATE75_128X64:
            width = 128;
            height = 64;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        case DISPLAY_INTERSTATE75_192X64:
            width = 192;
            height = 64;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        case DISPLAY_INTERSTATE75_256X64:
            width = 256;
            height = 64;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        case DISPLAY_INKY_FRAME_7:
            width = 800;
            height = 480;
            bus_type = BUS_SPI;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_INKY7;
            break;
        case DISPLAY_COSMIC_UNICORN:
            width = 32;
            height = 32;
            bus_type = BUS_PIO;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_RGB888;
            break;
        default:
            return false;
    }
    return true;
}

size_t get_required_buffer_size(PicoGraphicsPenType pen_type, uint width, uint height) {
    switch(pen_type) {
        case PEN_1BIT:
            return PicoGraphics_Pen1Bit::buffer_size(width, height);
        case PEN_3BIT:
            return PicoGraphics_Pen3Bit::buffer_size(width, height);
        case PEN_P4:
            return PicoGraphics_PenP4::buffer_size(width, height);
        case PEN_P8:
            return PicoGraphics_PenP8::buffer_size(width, height);
        case PEN_RGB332:
            return PicoGraphics_PenRGB332::buffer_size(width, height);
        case PEN_RGB565:
            return PicoGraphics_PenRGB565::buffer_size(width, height);
        case PEN_RGB888:
            return PicoGraphics_PenRGB888::buffer_size(width, height);
        case PEN_INKY7:
            return PicoGraphics_PenInky7::buffer_size(width, height);
        default:
            return 0;
    }
}

mp_obj_t ModPicoGraphics_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    ModPicoGraphics_obj_t *self = nullptr;

    enum { ARG_display, ARG_rotate, ARG_bus, ARG_buffer, ARG_pen_type, ARG_extra_pins, ARG_i2c_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_rotate, MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_bus, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_buffer, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_pen_type, MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_extra_pins, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_i2c_address, MP_ARG_INT, { .u_int = -1 } },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj_with_finaliser(ModPicoGraphics_obj_t);
    self->base.type = &ModPicoGraphics_type;

    PicoGraphicsDisplay display = (PicoGraphicsDisplay)args[ARG_display].u_int;

    bool round = display == DISPLAY_ROUND_LCD_240X240;
    int width = 0;
    int height = 0;
    int pen_type = args[ARG_pen_type].u_int;
    int rotate = args[ARG_rotate].u_int;
    PicoGraphicsBusType bus_type = BUS_SPI;
    if(!get_display_settings(display, width, height, rotate, pen_type, bus_type)) mp_raise_ValueError("Unsupported display!");
    if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
    
    pimoroni::SPIPins spi_bus = get_spi_pins(BG_SPI_FRONT);
    pimoroni::ParallelPins parallel_bus = {10, 11, 12, 13, 14, 2}; // Default for Tufty 2040 parall