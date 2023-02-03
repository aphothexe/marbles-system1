#include "vl53l5cx.h"


MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX___del___obj, VL53L5CX___del__);

MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX_start_ranging_obj, VL53L5CX_start_ranging);
MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX_stop_ranging_obj, VL53L5CX_stop_ranging);

MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_enable_motion_indicator_obj, VL53L5CX_enable_motion_indicator);
MP_DEFINE_CONST_FUN_OBJ_3(VL53L5CX_set_motion_distance_obj, VL53L5CX_set_motion_distance);

MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_i2c_address_obj, VL53L5CX_set_i2c_address);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_ranging_mode_obj, VL53L5CX_set_ranging_mode);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_ranging_frequency_hz_obj, VL53L5CX_set_ranging_frequency_hz);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_resolution_obj, VL53L5CX_set_resolution);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_integration_time_ms_obj, VL53L5CX_set_integration_time_ms);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_sharpener_percent_obj, VL53L5CX_set_sharpener_percent);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_target_order_obj, VL53L5CX_set_target_order);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_power_mode_obj, VL53L5CX_set_power_mode);

MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX_data_ready_obj, VL53L5CX_data_ready);
MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX_get_data_obj, VL53L5CX_get_data);


/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t VL53L5CX_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&VL53L5CX___del___obj) },

    { MP_ROM_QSTR(MP_QSTR_start_ranging), MP_ROM_PTR(&VL53L5CX_start_ranging_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_ranging), MP_ROM_PTR(&VL53L5CX_stop_ranging_obj) },

    { MP_ROM_QSTR(MP_QSTR_enable_motion_indicator), MP_ROM_PTR(&VL53L5CX_enable_motion_indicator_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_motion_distance), MP_ROM_PTR(&VL53L5CX_set_motion_distance_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_i2c_address), MP_ROM_PTR(&VL53L5CX_set_i2c_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_ranging_mode), MP_ROM_PTR(&VL53L5CX_set_ranging_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_ranging_frequency_hz), MP_ROM_PTR(&VL53L5CX_set_ranging_frequency_hz_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_resolution), MP_ROM_PTR(&VL53L5CX_set_resolution_obj) },
    { MP_ROM_QSTR(MP_