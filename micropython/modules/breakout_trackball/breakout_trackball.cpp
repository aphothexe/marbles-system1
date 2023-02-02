#include "libraries/breakout_trackball/breakout_trackball.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>


using namespace pimoroni;

extern "C" {
#include "breakout_trackball.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_trackball_BreakoutTrackball_obj_t {
    mp_obj_base_t base;
    BreakoutTrackball *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_trackball_BreakoutTrackball_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutTrackball_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_trackball_BreakoutTrackball_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address, ARG_interrupt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BreakoutTrackball::DEFAULT_I2C_ADDRESS} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_trackball_BreakoutTrackball_obj_t);
    self->base.type = &breakout_trackball_BreakoutTrackball_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutTrackball, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_address].u_int, args[ARG_interrupt].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "Trackball breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutTrackball_change_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_trackball_BreakoutTrackball_obj_t);

    self->breakout->change_address(args[ARG_address].u_int);

    return mp_const_none;
}

mp_obj_t BreakoutTrackball_enable_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_interrupt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_interrupt, MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_trackball_BreakoutTrackball_obj_t);

    self->breakout->enable_interrupt(args[ARG_interrupt].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutTrackball_get_interrupt(mp_obj_t self_in) {
    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_trackball_BreakoutTrackball_obj_t);
    return mp_obj_new_bool(self->breakout->get_interrupt());
}


mp_obj_t BreakoutTrackball_set_rgbw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_r, ARG_g, ARG_b, ARG_w };
    stati