#include "common/pimoroni_i2c.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>


using namespace pimoroni;

extern "C" {
#include "pimoroni_i2c.h"
#include "py/mperrno.h"
#include "extmod/machine_i2c.h"


_PimoroniI2C_obj_t*  PimoroniI2C_from_machine_i2c_or_native(mp_obj_t i2c_obj) {
    if(MP_OBJ_IS_TYPE(i2c_obj, &PimoroniI2C_type)) {
        return (_PimoroniI2C_obj_t *)MP_OBJ_TO_PTR(i2c_obj);
    } else if(MP_OBJ_IS_TYPE(i2c_obj, &machine_i2c_type)) {
        _PimoroniI2C_obj_t *pimoroni_i2c = m_new_obj(_PimoroniI2C_obj_t);
        machine_i2c_obj_t *machine_i2c = (machine_i2c_obj_t *)MP_OBJ_TO_PTR(i2c_obj);
        pimoroni_i2c = m_new_obj(_PimoroniI2C_obj_t);
        pimoroni_i2c->base.type = &PimoroniI2C_type;

        pimoroni_i2c->i2c = m_new_class(I2C, machine_i2c->sda, machine_i2c->scl, machine_i2c->freq);
        return pimoroni_i2c;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Bad I2C object"));
        return nullptr;
    }
}

void PimoroniI2C_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _PimoroniI2C_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PimoroniI2C_obj_t);
    I2C* i2c = (I2C*)self->i2c;
    mp_print_str(print, "PimoroniI2C(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((i2c->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(i2c->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(i2c->get_scl()), PRINT_REPR);

    mp_print_str(print, ")");
}

mp_obj_t PimoroniI2C_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PimoroniI2C_obj_t *self = nullptr;

    en