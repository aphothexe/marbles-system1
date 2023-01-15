#include "breakout_msa301.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutMSA301 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMSA301_part_id_obj, BreakoutMSA301_part_id);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_get_axis_obj, 2, BreakoutMSA301_get_axis);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_get_x_axis_obj, 1, BreakoutMSA301_get_x_axis);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_get_y_axis_obj, 1, BreakoutMSA301_get_y_axis);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_get_z_axis_obj, 1, BreakoutMSA301_get_z_axis);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMSA301_get_orientation_obj, BreakoutMSA301_get_orientation);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_set_power_mode_obj, 2, BreakoutMSA301_set_power_mode);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_set_range_and_resolution_obj, 2, BreakoutMSA301_set_range_and_resolution);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_set_axis_polarity_obj, 2, BreakoutMSA301_set_axis_polarity);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMSA301_disable_all_interrupts_obj, BreakoutMSA301_disable_all_interrupts);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_enable_interrupts_obj, 2, BreakoutMSA301_enable_interrupts);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_set_interrupt_latch_obj, 3, BreakoutMSA301_set_interrupt_latch);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_read_interrupt_obj, 2, BreakoutMSA301_read_interrupt);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutMSA301_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_part_id), MP_ROM_PTR(&BreakoutMSA301_part_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_axis), MP_ROM_PTR(&BreakoutMSA301_get_axis_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_x_axis), MP_ROM_PTR(&BreakoutMSA301_get_x_axis_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_y_axis), MP_ROM_PTR(&BreakoutMSA301_get_y