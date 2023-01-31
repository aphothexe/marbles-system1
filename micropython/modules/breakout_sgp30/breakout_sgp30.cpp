#include "libraries/breakout_sgp30/breakout_sgp30.hpp"
#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "breakout_sgp30.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_sgp30_BreakoutSGP30_obj_t {
    mp_obj_base_t base;
    BreakoutSGP30 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_sgp30_BreakoutSGP30_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutSGP30_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = nullptr;

    enum { ARG_i2c };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_sgp30_BreakoutSGP30_obj_t);
    self->base.type = &breakout_sgp30_BreakoutSGP30_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutSGP30, (pimoroni::I2C *)(self->i2c->i2c));

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutSGP30: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutSGP30_retrieve_unique_id(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    return mp_obj_new_bool(self->breakout->retrieve_unique_id());
}

mp_obj_t BreakoutSGP30_get_unique_id(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    uint64_t id = self->brea