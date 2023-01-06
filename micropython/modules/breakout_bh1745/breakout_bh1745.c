#include "breakout_bh1745.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutBH1745 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_chip_id_obj, BreakoutBH1745_chip_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_manufacturer_id_obj, BreakoutBH1745_manufacturer_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_rgbc_raw_obj, BreakoutBH1745_rgbc_raw);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_rgbc_clamped_obj, BreakoutBH1745_rgbc_clamped);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_rgbc_scaled_obj, BreakoutBH1745_rgbc_scaled);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutBH1745_leds_obj, 2, BreakoutBH1745_leds);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutBH1745_threshold_obj, 3, BreakoutBH1745_threshold);
MP_DEFINE_CONST_FUN_OBJ_KW(Brea