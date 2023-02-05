#include "drivers/encoder/encoder.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>
#include <cfloat>


using namespace pimoroni;
using namespace encoder;

extern "C" {
#include "encoder.h"
#include "py/builtin.h"


/********** Encoder **********/

/***** Variables Struct *****/
typedef struct _Encoder_obj_t {
    mp_obj_base_t base;
    Encoder* encoder;
} _Encoder_obj_t;


/***** Print *****/
void Encoder_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    mp_print_str(print, "Encoder(");

    mp_print_str(print, "pins = (");
    pin_pair pins = self->encoder->pins();
    mp_obj_print_helper(print, mp_obj_new_int(pins.a), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(pins.b), PRINT_REPR);
    mp_print_str(print, ", ");
    uint common_pin = self->encoder->common_pin();
    if(common_pin == PIN_UNUSED)
        mp_print_str(print, "PIN_UNUSED");
    else
        mp_obj_print_helper(print, mp_obj_new_int(self->encoder->common_pin()), PRINT_REPR);
    mp_print_str(print, ")");

    if(self->encoder->direction() == NORMAL_DIR)
        mp_print_str(print, ", direction = NORMAL_DIR");
    else
        mp_print_str(print, ", direction = REVERSED_DIR");

    mp_print_str(print, ", counts_per_rev = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->encoder->counts_per_rev()), PRINT_REPR);
    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t Encoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, 