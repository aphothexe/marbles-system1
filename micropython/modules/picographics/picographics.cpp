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
    pimoroni::ParallelPins parallel_bus = {10, 11, 12, 13, 14, 2}; // Default for Tufty 2040 parallel
    pimoroni::I2C *i2c_bus = nullptr;

    if (mp_obj_is_type(args[ARG_bus].u_obj, &SPIPins_type)) {
        if(bus_type != BUS_SPI) mp_raise_ValueError("unexpected SPI bus!");
        _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
        spi_bus = *(SPIPins *)(bus->pins);

    } else if (mp_obj_is_type(args[ARG_bus].u_obj, &ParallelPins_type)) {
        if(bus_type != BUS_PARALLEL) mp_raise_ValueError("unexpected Parallel bus!");
        _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
        parallel_bus = *(ParallelPins *)(bus->pins);

    } else if (mp_obj_is_type(args[ARG_bus].u_obj, &PimoroniI2C_type) || MP_OBJ_IS_TYPE(args[ARG_bus].u_obj, &machine_i2c_type)) {
        if(bus_type != BUS_I2C) mp_raise_ValueError("unexpected I2C bus!");
        self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_bus].u_obj);
        i2c_bus = (pimoroni::I2C *)(self->i2c->i2c);

    } else {
        // No bus given, fall back to defaults
        if(bus_type == BUS_I2C) {
            self->i2c = (_PimoroniI2C_obj_t *)MP_OBJ_TO_PTR(PimoroniI2C_make_new(&PimoroniI2C_type, 0, 0, nullptr));
            i2c_bus = (pimoroni::I2C *)(self->i2c->i2c);
        } else if (bus_type == BUS_SPI) {
            if(display == DISPLAY_INKY_FRAME || display == DISPLAY_INKY_FRAME_4 || display == DISPLAY_INKY_FRAME_7) {
                spi_bus = {PIMORONI_SPI_DEFAULT_INSTANCE, SPI_BG_FRONT_CS, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, 28, PIN_UNUSED};
            } else if (display == DISPLAY_INKY_PACK) {
                spi_bus = {PIMORONI_SPI_DEFAULT_INSTANCE, SPI_BG_FRONT_CS, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, 20, PIN_UNUSED};
            } else if (display == DISPLAY_GFX_PACK) {
                spi_bus = {PIMORONI_SPI_DEFAULT_INSTANCE, 17, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, 20, 9};
            }
        }
    }

    // Try to create an appropriate display driver
    if (display == DISPLAY_INKY_FRAME || display == DISPLAY_INKY_FRAME_4) {
        pen_type = PEN_3BIT; // FORCE to 3BIT
        // TODO grab BUSY and RESET from ARG_extra_pins
        self->display = m_new_class(UC8159, width, height, (Rotation)rotate, spi_bus);

    } else if (display == DISPLAY_INKY_FRAME_7) {
        pen_type = PEN_INKY7;
        // TODO grab BUSY and RESET from ARG_extra_pins
        self->display = m_new_class(Inky73, width, height, (Rotation)rotate, spi_bus);

    } else if (display == DISPLAY_TUFTY_2040) {
        self->display = m_new_class(ST7789, width, height, (Rotation)rotate, parallel_bus);

    } else if (display == DISPLAY_LCD_160X80) {
        self->display = m_new_class(ST7735, width, height, spi_bus);

    } else if (display == DISPLAY_I2C_OLED_128X128) {
        int i2c_address = args[ARG_i2c_address].u_int;
        if(i2c_address == -1) i2c_address = SH1107::DEFAULT_I2C_ADDRESS;

        self->display = m_new_class(SH1107, width, height, *i2c_bus, (uint8_t)i2c_address);

    } else if (display == DISPLAY_INKY_PACK) {
        self->display = m_new_class(UC8151, width, height, (Rotation)rotate, spi_bus);

    } else if (display == DISPLAY_GALACTIC_UNICORN) {
        self->display = m_new_class(DisplayDriver, width, height, (Rotation)rotate);
    
    } else if (display == DISPLAY_GFX_PACK) {
        self->display = m_new_class(ST7567, width, height, spi_bus);

    } else if (display == DISPLAY_INTERSTATE75_32X32 || display == DISPLAY_INTERSTATE75_64X64 || display == DISPLAY_INTERSTATE75_64X32) {
        self->display = m_new_class(DisplayDriver, width, height, (Rotation)rotate);
    
    } else if (display == DISPLAY_COSMIC_UNICORN) {
        self->display = m_new_class(DisplayDriver, width, height, (Rotation)rotate);

    } else {
        self->display = m_new_class(ST7789, width, height, (Rotation)rotate, round, spi_bus);
    }

    // Create or fetch buffer
    size_t required_size = get_required_buffer_size((PicoGraphicsPenType)pen_type, width, height);
    if(required_size == 0) mp_raise_ValueError("Unsupported pen type!");

    if(pen_type == PEN_INKY7) {
        self->buffer = m_new_class(PSRamDisplay, width, height);
    } else {
        if (args[ARG_buffer].u_obj != mp_const_none) {
            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
            self->buffer = bufinfo.buf;
            if(bufinfo.len < (size_t)(required_size)) {
                mp_raise_ValueError("Supplied buffer is too small!");
            }
        } else {
            self->buffer = m_new(uint8_t, required_size);
        }
    }

    // Create an instance of the graphics library
    // use the *driver* width/height because they may have been swapped due to rotation
    switch((PicoGraphicsPenType)pen_type) {
        case PEN_1BIT:
            if (display == DISPLAY_INKY_PACK) {
                self->graphics = m_new_class(PicoGraphics_Pen1BitY, self->display->width, self->display->height, self->buffer);
            } else {
                self->graphics = m_new_class(PicoGraphics_Pen1Bit, self->display->width, self->display->height, self->buffer);
            }
            break;
        case PEN_3BIT:
            self->graphics = m_new_class(PicoGraphics_Pen3Bit, self->display->width, self->display->height, self->buffer);
            break;
        case PEN_P4:
            self->graphics = m_new_class(PicoGraphics_PenP4, self->display->width, self->display->height, self->buffer);
            break;
        case PEN_P8:
            self->graphics = m_new_class(PicoGraphics_PenP8, self->display->width, self->display->height, self->buffer);
            break;
        case PEN_RGB332:
            self->graphics = m_new_class(PicoGraphics_PenRGB332, self->display->width, self->display->height, self->buffer);
            break;
        case PEN_RGB565:
            self->graphics = m_new_class(PicoGraphics_PenRGB565, self->display->width, self->display->height, self->buffer);
            break;
        case PEN_RGB888:
            self->graphics = m_new_class(PicoGraphics_PenRGB888, self->display->width, self->display->height, self->buffer);
            break;
        case PEN_INKY7:
            self->graphics = m_new_class(PicoGraphics_PenInky7, self->display->width, self->display->height, *(IDirectDisplayDriver<uint8_t> *)self->buffer);
            break;
        default:
            break;
    }

    //self->scanline_callback = mp_const_none;

    self->spritedata = nullptr;

    // Clear the buffer
    self->graphics->set_pen(0);
    self->graphics->clear();

    // Update the LCD from the graphics library
    if (display != DISPLAY_INKY_FRAME && display != DISPLAY_INKY_FRAME_4 && display != DISPLAY_INKY_PACK && display != DISPLAY_INKY_FRAME_7) {
        self->display->update(self->graphics);
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t ModPicoGraphics__del__(mp_obj_t self_in) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    self->display->cleanup();
    return mp_const_none;
}

mp_obj_t ModPicoGraphics_set_spritesheet(mp_obj_t self_in, mp_obj_t spritedata) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    if(spritedata == mp_const_none) {
        self->spritedata = nullptr;
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(spritedata, &bufinfo, MP_BUFFER_RW);

        int required_size = get_required_buffer_size((PicoGraphicsPenType)self->graphics->pen_type, 128, 128);

        if(bufinfo.len != (size_t)(required_size)) {
            mp_raise_ValueError("Spritesheet the wrong size!");
        }

        self->spritedata = bufinfo.buf;
    }
    return mp_const_none;
}

mp_obj_t ModPicoGraphics_load_spritesheet(mp_obj_t self_in, mp_obj_t filename) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    mp_obj_t args[2] = {
        filename,
        MP_OBJ_NEW_QSTR(MP_QSTR_r),
    };

    // Stat the file to g