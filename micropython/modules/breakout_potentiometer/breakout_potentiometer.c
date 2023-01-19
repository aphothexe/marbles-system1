#include "breakout_potentiometer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutPotentiometer Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_address_obj, 2, BreakoutPotentiometer_set_address);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPotentiometer_get_adc_vref_obj, BreakoutPotentiometer_get_adc_vref);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_adc_vref_obj, 2, BreakoutPotentiometer_set_adc_vref);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPotentiometer_get_direction_obj, BreakoutPotentiometer_get_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_direction_obj, 2, BreakoutPotentiometer_set_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_brightness_obj, 2, BreakoutPotentiometer_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_led_obj, 4, BreakoutPotentiometer_set_led);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPotentiometer_read_obj, BreakoutPotentiometer_read);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPotentiometer_read_raw_obj, BreakoutPotentiometer_read_raw);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutPotentiometer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_address), MP_ROM_PTR(&BreakoutPotentiometer_set_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_adc_vref), MP_ROM_PTR(&BreakoutPotentiometer_get_adc_vref_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_adc_vref), MP_ROM_PTR(&BreakoutPotentiometer_set_adc_v