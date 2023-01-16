#include "breakout_pmw3901.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutPMW3901 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPAA5100_get_id_obj, BreakoutPMW3901_get_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPAA5100_get_revision_obj, BreakoutPMW3901_get_revision);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPAA5100_set_rotation_obj, 1, BreakoutPMW3901_set_rotation);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPAA5100_set_orientation_obj, 1, BreakoutPMW3901_set_orientation);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPAA5100_get_motion_obj, 1, BreakoutPMW3901_get_motion);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPAA5100_get_motion_slow_obj, 1, BreakoutPMW3901_get_motion_slow);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPAA5100_frame_capture_obj, 2, BreakoutPMW3901_frame_capture);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutPAA5100_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_id), MP_ROM_PTR(&BreakoutPAA5100_get_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_revision), MP_ROM_PTR(&BreakoutPAA5100_get_revision_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_rotation), MP_ROM_PTR(&BreakoutPAA5100_set_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_orientation), MP_ROM_PTR(&BreakoutPAA5100_set_orientation_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_motion), MP_ROM_PTR(&BreakoutPAA5100_get_motion_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_motion_slow), MP_ROM_PTR(&BreakoutPAA5100_get_motion_slow_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame_capture), MP_ROM_PTR(&BreakoutPAA5100_frame_capture_obj) },
    { MP_ROM_QSTR(MP_QSTR_DEGREES_0), MP_ROM_INT(0x00) },
    { MP_ROM_QSTR(MP_QSTR_DEGREES_90), MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_DEGREES_180), MP_ROM_INT(0x02) },
    { MP_ROM_QSTR(MP_QSTR_DEGREES_270), MP_ROM_INT(0x03) },
    { MP_ROM_QSTR(MP_QSTR_FRAME_SIZE), MP_ROM_INT(35) },
    { MP_ROM_QSTR(MP_QSTR_FRAME_BYTES), MP_ROM_INT(1225) 