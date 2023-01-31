#include "breakout_scd41.h"

/***** Constants *****/


////////////////////////////////////////////////////////////////////////////////////////////////////
// SCD41 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
// Init, optionally (though you really should supply it) accepts a PimoroniI2C instance
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(scd41_init_obj, 0, scd41_init);

// Start/Stop measurement, no args (module-level, so no "self")
STATIC MP_DEFINE_CONST_FUN_OBJ_0(scd41_start_periodic_measurement_obj, scd41_start_periodic_measurement);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(scd41_stop_periodic_measurement_obj, scd41_stop_periodic_measurement);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(scd41_get_data_ready_obj, scd41_get_data_ready);

STATIC MP_DEFINE_CONST_FUN_OBJ_1(scd41_set_temperature_offset_obj, scd41_set_temperature_offset);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(scd41_get_temperature_offset_obj, scd41_get_temperature_offset);

STATIC MP_DEFINE_CONST_FUN_OBJ_1(scd41_set_sensor_altitude_obj, scd41_set_sensor_altitude);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(scd41_set_ambient_pressure_obj, scd41_set_ambient_pressure);

// No args here, either, we're home free!
STATIC MP_DEFINE_CONST_FUN_OBJ_0(scd41_read_measurement_obj, scd41_read_measurement);

/***** Globals Table *****/
STATIC const mp_map_elem_t scd41_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_breakout_scd41) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&scd41_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&scd41_start_periodic_measurement_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&scd41_stop_periodic_measurement_obj) },
    { MP_ROM_QSTR(MP_QSTR_measure), MP_ROM_PTR(&scd41_read_measurement_obj) },
    { MP_ROM_QSTR(MP_QSTR_ready), MP_ROM_PTR(&scd41_get_data_ready_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_temperature_offset), MP_ROM_PTR(&scd41_set_temperature_offset_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_temperature_offset), MP_ROM_PTR(&scd41_get_temperature_offset_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_sensor_altitude), 