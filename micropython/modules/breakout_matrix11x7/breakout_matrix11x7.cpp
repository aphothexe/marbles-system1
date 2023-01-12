#include "libraries/breakout_matrix11x7/breakout_matrix11x7.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>


using namespace pimoroni;

extern "C" {
#include "breakout_matrix11x7.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_matrix11x7_BreakoutMatrix11x7_obj_t {
    mp_obj_base_t base;
    BreakoutMatrix11x7 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_matrix11x7_BreakoutMatrix11x7_obj_t;

/***** Constructor *****/
mp_obj_t BreakoutMatrix11x7_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_matrix11x7_BreakoutMatrix11x7_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BreakoutMatrix11x7::DEFAULT_I2C_ADDRESS} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_matrix11x7_BreakoutMatrix11x7_obj_t);
    self->base.type = &breakout_matrix11x7_BreakoutMatrix11x7_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutMatrix11x7, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_address].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutMatrix11x7: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutMatrix11x7_set_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_val };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_col, MP_ARG_REQUIRED | MP_ARG_INT },
    