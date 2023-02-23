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
            