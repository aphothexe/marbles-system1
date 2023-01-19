#include "libraries/breakout_pmw3901/breakout_pmw3901.hpp"
#include "libraries/breakout_paa5100/breakout_paa5100.hpp"
#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "breakout_pmw3901.h"
#include <cstring>

/***** Variables Struct *****/
typedef struct _breakout_pmw3901_BreakoutPMW3901_obj_t {
    mp_obj_base_t base;
    BreakoutPMW3901 *breakout;
    ChipType chip;
} breakout_pmw3901_BreakoutPMW3901_obj_t;

/***** Print *****/
void BreakoutPMW3901_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    breakout_pmw3901_BreakoutPMW3901_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_pmw3901_BreakoutPMW3901_obj_t);
    BreakoutPMW3901* breakout = self->breakout;

    if(self->chip == ChipType::PMW3901) {
        mp_print_str(print, "BreakoutPMW3901(");
    } else {
        mp_print_str(print, "BreakoutPAA5100(");
    }

    mp_print_str(print, "spi = ");
    mp_obj_print_helper(print, mp_obj_new_int((breakout->get_spi() == spi0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", cs = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_cs()), PRINT_REPR);

    mp_print_str(print, ", sck = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_sck()), PRINT_REPR);

    mp_print_str(print, ", mosi = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_mosi()), PRINT_REPR);

    mp_print_str(print, ", miso = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_miso()), PRINT_REPR);

    mp_print_str(print, ", int = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_int()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/

mp_obj_t BreakoutPMW3901_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    return make_new(ChipType::PMW3901, type, n_args, n_kw, all_args);
}

mp_obj_t BreakoutPAA5100_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    return make_new(ChipType::PAA5100, type, n_args, n_kw, all_args);
}

mp_obj_t make_new(enum ChipType chip, const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_pmw3901_BreakoutPMW3901_obj_t *self = nullptr;

    if(n_args + n_kw == 1) {
        enum { ARG_slot };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_slot, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all_k