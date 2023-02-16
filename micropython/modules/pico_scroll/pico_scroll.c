#include "pico_scroll.h"

/***** Constants *****/
enum buttons
{
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// picoscroll Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoscroll_init_obj, picoscroll_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoscroll_get_width_obj, picoscroll_get_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoscroll_get_height_obj, picoscroll_get_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoscroll_update_obj, picoscroll_update);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picoscroll_set_pixel_obj, picoscroll_set_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoscroll_set_pixels_obj, picoscroll_set_pixels);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picoscroll_show_text_obj, picoscroll_show_text);
