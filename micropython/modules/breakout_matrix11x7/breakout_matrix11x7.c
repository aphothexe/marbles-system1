#include "breakout_matrix11x7.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutMatrix11x7 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMatrix11x7_set_pixel_obj, 1, BreakoutMatrix11x7_set_pixel);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMatrix11x7_update_obj, BreakoutMatrix11x7_update);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMatrix11x7_clear_obj, BreakoutMatrix11x7_clear);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutMatrix11x7_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&BreakoutMatrix11x7_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&BreakoutMatrix11x7_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutMatrix11x7_clear_obj) },
    { MP