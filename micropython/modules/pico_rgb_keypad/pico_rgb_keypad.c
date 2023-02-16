#include "pico_rgb_keypad.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// picokeypad Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_init_obj, picokeypad_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_get_width_obj, picokeypad_get_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_get_height_obj, picokeypad_get_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_get_num_pads_obj, picokeypad_get_num_pads);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_update_obj, picokeypad_update);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picokeypad_set_brightness_obj, picokeypad_set_brightness);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picokeypad_illuminate_xy_obj, 5, 5, picokeypad_illuminate_xy);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picokeypad_illuminate_obj, 4, 4, picokeypad_illuminate);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_clear_obj, picokeypad_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_get_button_states_obj, picokeypad_get_button_states);

/***** Globals Table *****/
STATIC const mp_map_elem_t picokeypad_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picokeypad) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&picokeypad_init_obj) },    
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&picokeypad_get_width_obj) },
    {