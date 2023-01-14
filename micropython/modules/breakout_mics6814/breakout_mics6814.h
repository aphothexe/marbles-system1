// Include MicroPython API.
#include "py/runtime.h"

enum {
    REF = 0,
    REDUCING,
    NH3,
    OXIDISING
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_mics6814_BreakoutMICS6814_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutMICS6814_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutMICS6814_set_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMICS6814_get_adc_vref(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_set_adc_vref(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp