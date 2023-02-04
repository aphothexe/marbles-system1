#include "cosmic_unicorn.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Channel___del___obj, Channel___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(Channel_configure_obj, 1, Channel_configure);
MP_DEFINE_CONST_FUN_OBJ_1(Channel_restore_obj, Channel_restore);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(Channel_waveforms_obj, 1, 2, Channel_waveforms);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(Channel_frequency_obj, 1, 2, Channel_frequency);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(Channel_volume_obj, 1, 2, Channel_volume);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(Channel_attack_duration_obj, 1, 2, Channel_attack_duration);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(Channel_decay_duration_obj, 1, 2, Channel_decay_duration);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(Channel_sustain_level_obj, 1, 2, Channel_sustain_level);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(Channel_release_duration_obj, 1, 2, Channel_release_duration);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(Channel_pulse_width_obj, 1, 2, Channel_pulse_width);
MP_DEFINE_CONST_FUN_OBJ_1(Channel_trigger_attack_obj, Channel_trigger_attack);
MP_DEFINE_CONST_FUN_OBJ_1(Channel_trigger_release_obj, Channel_trigger_release);
MP_DEFINE_CONST_FUN_OBJ_KW(Channel_play_tone_obj, 2, Channel_play_tone);
//MP_DEFINE_CONST_FUN_OBJ_1(Channel_stop_playing_obj, Channel_stop_playing);

MP_DEFINE_CONST_FUN_OBJ_1(CosmicUnicorn___del___obj, CosmicUnicorn___del__);
MP_DEFINE_CONST_FUN_OBJ_1(CosmicUnicorn_clear_obj, CosmicUnicorn_clear);
MP_DEFINE_CONST_FUN_OBJ_2(CosmicUnicorn_update_obj, CosmicUnicorn_update);
MP_DEFINE_CONST_FUN_OBJ_2(CosmicUnicorn_set_brightness_obj, CosmicUnicorn_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_1(CosmicUnicorn_get_brightness_obj, CosmicUnicorn_get_brightness);
MP_DEFINE_CONST_FUN_OBJ_2(CosmicUnicorn_adjust_brightness_obj, CosmicUnicorn_adjust_brightness);
MP_DEFINE_CONST_FUN_OBJ_2(CosmicUnicorn_set_volume_obj, CosmicUnicorn_set_volume);
MP_DEFINE_CONST_FUN_OBJ_1(CosmicUnicorn_get_volume_obj, CosmicUnicorn_get_volume);
MP_DEFINE_CONST_FUN_OBJ_2(CosmicUnicorn_adjust_volume_obj, CosmicUnicorn_adjust_volume);
MP_DEFINE_CONST_FUN_OBJ_1(CosmicUnicorn_light_obj, CosmicUnicorn_light);
MP_DEFINE_CONST_FUN_OBJ_2(CosmicUnicorn_is_pressed_obj, CosmicUnicorn_is_pressed);
MP_DEFINE_CONST_FUN_OBJ_2(CosmicUnicorn_play_sample_obj, CosmicUnicorn_play_sample);
MP_DEFINE_CONST_FUN_OBJ_1(CosmicUnicorn_play_synth_obj, CosmicUnicorn_play_synth);
MP_DEFINE_CONST_FUN_OBJ_1(CosmicUnicorn_stop_playing_obj, CosmicUnicorn_stop_playing);
MP_DEFINE_CONST_FUN_OBJ_2(CosmicUnicorn_synth_channel_obj, CosmicUnicorn_synth_channel);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Channel_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Channel___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&Channel_configure_obj) },
    { MP_ROM_QSTR(MP_QSTR_restore), MP_ROM_PTR(&Channel_restore_obj) },
    { MP_ROM_QSTR(MP_QSTR_waveforms), MP_ROM_PTR(&Channel_waveforms_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&Channel_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_volume), MP_ROM_PTR(&Channel_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_attack_duration), MP_ROM_PTR(&Channel_attack_duration_obj) },
    { MP_ROM_QSTR(MP_QSTR_decay_duration), MP_ROM_PTR(&Channel_decay_duration_obj) },
    { MP_ROM_QSTR(MP_QSTR_sustain_level), MP_ROM_PTR(&Channel_sustain_level_obj) },
    { MP_ROM_QSTR(MP_QSTR_release_duratio