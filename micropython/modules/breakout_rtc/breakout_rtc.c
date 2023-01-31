
#include "breakout_rtc.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutRTC Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_reset_obj, BreakoutRTC_reset);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_setup_obj, BreakoutRTC_setup);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_set_time_obj, 8, BreakoutRTC_set_time);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_seconds_obj, BreakoutRTC_set_seconds);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_minutes_obj, BreakoutRTC_set_minutes);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_hours_obj, BreakoutRTC_set_hours);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_weekday_obj, BreakoutRTC_set_weekday);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_date_obj, BreakoutRTC_set_date);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_month_obj, BreakoutRTC_set_month);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_year_obj, BreakoutRTC_set_year);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_set_to_compiler_time_obj, BreakoutRTC_set_to_compiler_time);

MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_update_time_obj, BreakoutRTC_update_time);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_string_date_usa_obj, BreakoutRTC_string_date_usa);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_string_date_obj, BreakoutRTC_string_date);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_string_time_obj, BreakoutRTC_string_time);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_string_time_stamp_obj, BreakoutRTC_string_time_stamp);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_seconds_obj, BreakoutRTC_get_seconds);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_minutes_obj, BreakoutRTC_get_minutes);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_hours_obj, BreakoutRTC_get_hours);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_weekday_obj, BreakoutRTC_get_weekday);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_date_obj, BreakoutRTC_get_date);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_month_obj, BreakoutRTC_get_month);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_year_obj, BreakoutRTC_get_year);

MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_is_12_hour_obj, BreakoutRTC_is_12_hour);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_is_pm_obj, BreakoutRTC_is_pm);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_set_12_hour_obj, BreakoutRTC_set_12_hour);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_set_24_hour_obj, BreakoutRTC_set_24_hour);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_unix_obj, BreakoutRTC_set_unix);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_unix_obj, BreakoutRTC_get_unix);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_enable_alarm_interrupt_obj, 6, BreakoutRTC_enable_alarm_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_alarm_interrupt_obj, BreakoutRTC_disable_alarm_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_read_alarm_interrupt_flag_obj, BreakoutRTC_read_alarm_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_clear_alarm_interrupt_flag_obj, BreakoutRTC_clear_alarm_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_set_timer_obj, 6, BreakoutRTC_set_timer);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_timer_count_obj, BreakoutRTC_get_timer_count);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_enable_timer_obj, BreakoutRTC_enable_timer);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_timer_obj, BreakoutRTC_disable_timer);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_enable_timer_interupt_obj, BreakoutRTC_enable_timer_interupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_timer_interrupt_obj, BreakoutRTC_disable_timer_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_read_timer_interrupt_flag_obj, BreakoutRTC_read_timer_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_clear_timer_interrupt_flag_obj, BreakoutRTC_clear_timer_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_enable_periodic_update_interrupt_obj, 2, BreakoutRTC_enable_periodic_update_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_periodic_update_interrupt_obj, BreakoutRTC_disable_periodic_update_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_read_periodic_update_interrupt_flag_obj, BreakoutRTC_read_periodic_update_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_clear_periodic_update_interrupt_flag_obj, BreakoutRTC_clear_periodic_update_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_enable_trickle_charge_obj, 2, BreakoutRTC_enable_trickle_charge);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_trickle_charge_obj, BreakoutRTC_disable_trickle_charge);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_backup_switchover_mode_obj, BreakoutRTC_set_backup_switchover_mode);