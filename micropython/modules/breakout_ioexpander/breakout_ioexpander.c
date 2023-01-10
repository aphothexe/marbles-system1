#include "breakout_ioexpander.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutIOExpander Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutIOExpander_get_chip_id_obj, BreakoutIOExpander_get_chip_id);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_set_address_obj, 2, BreakoutIOExpander_set_address);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutIOExpander_get_adc_vref_obj, BreakoutIOExpander_get_adc_vref);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_set_adc_vref_obj, 2, BreakoutIOExpander_set_adc_vref);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_enable_interrupt_out_obj, 1, BreakoutIOExpander_enable_interrupt_out);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutIOExpander_disable_interrupt_out_obj, BreakoutIOExpander_disable_interrupt_out);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutIOExpander_get_interrupt_flag_obj, BreakoutIOExpander_get_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutIOExpander_clear_interrupt_flag_obj, BreakoutIOExpander_clear_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_set_pin_interrupt_obj, 3, BreakoutIOExpander_set_pin_interrupt);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_pwm_load_obj, 1, BreakoutIOExpander_pwm_load);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutIOExpander_pwm_loading_obj, BreakoutIOExpander_pwm_loading);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_pwm_clear_obj, 1, BreakoutIOExpander_pwm_clear);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutIOExpander_pwm_clearing_obj, BreakoutIOExpander_pwm_clearing);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_set_pwm_control_obj, 2, BreakoutIOExpander_set_pwm_control);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_set_pwm_period_obj, 2, BreakoutIOExpander_set_pwm_period);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_set_mode_obj, 3, BreakoutIOExpander_set_mode);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_input_obj, 2, BreakoutIOExpander_input);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_input_as_voltage_obj, 2, BreakoutIOExpander_input_as_voltage);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_output_obj, 3, BreakoutIOExpander_output);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_setup_rotary_encoder_obj, 4, BreakoutIOExpander_setup_rotary_encoder);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutIOExpander_read_rotary_encoder_obj, 2, BreakoutIOExpander_read_rotary_encoder);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutIOExpander_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_chip_id), MP_ROM_PTR(&BreakoutIOExpander_get_chip_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_address), MP_ROM_PTR(&BreakoutIOExpander_set_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_adc_vref), MP_ROM_PTR(&BreakoutIOExpander_get_adc_vref_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_adc_vref), MP_ROM_PTR(&BreakoutIOExpander_set_adc_vref_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_interrupt_out), MP_ROM_PTR(&BreakoutIOExpander_enable_interrupt_out_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_interrupt_out), MP_ROM_PTR(&BreakoutIOExpander_disable_interrupt_out_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_interrupt_flag), MP_ROM_PTR(&BreakoutIOExpander_get_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_interrupt_flag), MP_ROM_PTR(&BreakoutIOExpander_clear_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pin_interrupt), MP_ROM_PTR(&BreakoutIOExpander_set_pin_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_pwm_load), MP_ROM_PTR(&BreakoutIOExpander_pwm_load_obj) },
    { MP_ROM_QSTR(MP_QSTR_pwm_loading), MP_ROM_PTR(&BreakoutIOExpander_pwm_loading_obj) },
    { MP_ROM_QSTR(MP_QSTR_pwm_clear), MP_ROM_PTR(&BreakoutIOExpander_pwm_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pwm_clearing), MP_ROM_PTR(&BreakoutIOExpander_pwm_clearing_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pwm_control), MP_ROM_PTR(&BreakoutIOExpander_set_pwm_control_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pwm_period), MP_ROM_PTR(&BreakoutIOExpander_set_pwm_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_mode), MP_ROM_PTR(&BreakoutIOExpander_set_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_input), MP_ROM_PTR(&BreakoutIOExpander_input_obj) },
    { MP_ROM_QSTR(MP_QSTR_input_as_voltage), MP_ROM_PTR(&BreakoutIOExpander_input_as_voltage_obj) },
    { MP_ROM_QSTR(MP_QSTR_output), MP_ROM_PTR(&BreakoutIOExpander_output_obj) },
    { MP_ROM_QSTR(MP_QSTR_setup_rotary_encoder), MP_ROM_PTR(&BreakoutIOExpander_setup_rotary_encoder_obj) },
    { MP_ROM_QSTR(MP