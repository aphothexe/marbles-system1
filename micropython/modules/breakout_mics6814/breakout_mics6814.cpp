#include "libraries/breakout_mics6814/breakout_mics6814.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>


using namespace pimoroni;

extern "C" {
#include "breakout_mics6814.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_mics6814_BreakoutMICS6814_obj_t {
    mp_obj_base_t base;
    BreakoutMICS6814 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_mics6814_BreakoutMICS6814_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutMICS6814_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address, ARG_interrupt };
        static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BreakoutMICS6814::DEFAULT_I2C_ADDRESS} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_mics6814_BreakoutMICS6814_obj_t);
    self->base.type = &breakout_mics6814_BreakoutMICS