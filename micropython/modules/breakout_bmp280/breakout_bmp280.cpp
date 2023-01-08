#include "drivers/bmp280/bmp280.hpp"
#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "breakout_bmp280.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_bmp280_BreakoutBMP280_obj_t {
    mp_obj_base_t base;
    BMP280 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_bmp280_BreakoutBMP280_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutBMP280_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_bmp280_BreakoutBMP280_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address, ARG_int };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BMP280::DEFAULT_I2C_ADDRESS} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_bmp280_BreakoutBMP280_obj_t);
    self->base.type = &breakout_bmp280_BreakoutBMP280_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BMP280, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_address].u_in