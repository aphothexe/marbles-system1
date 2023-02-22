#include "picographics.h"

// Module functions
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ModPicoGraphics_module_RGB_to_RGB332_obj, ModPicoGraphics_module_RGB_to_RGB332);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ModPicoGraphics_module_RGB_to_RGB565_obj, ModPicoGraphics_module_RGB_to_RGB565);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ModPicoGraphics_module_RGB332_to_RGB_obj, ModPicoGraphics_module_RGB332_to_RGB);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ModPicoGraphics_module_RGB565_to_RGB_obj, ModPicoGraphics_module_RGB565_to_RGB);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_get_required_buffer_size_obj, ModPicoGraphics_get_required_buffer_size);

// Class Methods
MP_DEFINE_CONST_FUN_OBJ_1(ModPicoGraphics_update_obj, ModPicoGraphics_update);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_partial_update_obj, 5, 5, ModPicoGraphics_partial_update);
MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_set_backlight_obj, ModPicoGraphics_set_backlight);
MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_set_update_speed_obj, ModPicoGraphics_set_update_speed);

// Palette management
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_update_pen_obj, 5, 5, ModPicoGraphics_update_pen);
MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_reset_pen_obj, ModPicoGraphics_reset_pen);
MP_DEFINE_CONST_FUN_OBJ_KW(ModPicoGraphics_set_palette_obj, 2, ModPicoGraphics_set_palette);

// Pen
MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_set_pen_obj, ModPicoGraphics_set_pen);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_create_pen_obj, 4, 4, ModPicoGraphics_create_pen);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_create_pen_hsv_obj, 4, 4, ModPicoGraphics_create_pen_hsv);
MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_set_thickness_obj, ModPicoGraphics_set_thickness);

// Primitives
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_set_clip_obj, 5, 5, ModPicoGraphics_set_clip);
MP_DEFINE_CONST_FUN_OBJ_1(ModPicoGraphics_remove_clip_obj, ModPicoGraphics_remove_clip);
MP_DEFINE_CONST_FUN_OBJ_1(ModPicoGraphics_clear_obj, ModPicoGraphics_clear);
MP_DEFINE_CONST_FUN_OBJ_3(ModPicoGraphics_pixel_obj, ModPicoGraphics_pixel);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_pixel_span_obj, 4, 4, ModPicoGraphics_pixel_span);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_rectangle_obj, 5, 5, ModPicoGraphics_rectangle);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_circle_obj, 4, 4, ModPicoGraphics_circle);
MP_DEFINE_CONST_FUN_OBJ_KW(ModPicoGraphics_character_obj, 1, ModPicoGraphics_character);
MP_DEFINE_CONST_FUN_OBJ_KW(ModPicoGraphics_text_obj, 1, ModPicoGraphics_text);
MP_DEFINE_CONST_FUN_OBJ_KW(ModPicoGraphics_measure_text_obj, 1, ModPicoGraphics_measure_text);
MP_DEFINE_CONST_FUN_OBJ_KW(ModPicoGraphics_polygon_obj, 2, ModPicoGraphics_polygon);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_triangle_obj, 7, 7, ModPicoGraphics_triangle);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_line_obj, 5, 6, ModPicoGraphics_line);

// Sprites
MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_set_spritesheet_obj, ModPicoGraphics_set_spritesheet);
MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_load_spritesheet_obj, ModPicoGraphics_load_spritesheet);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ModPicoGraphics_sprite_obj, 5, 7, ModPicoGraphics_sprite);

// Utility
//MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_set_scanline_callback_obj, ModPicoGraphics_set_scanline_callback);
MP_DEFINE_CONST_FUN_OBJ_1(ModPicoGraphics_get_bounds_obj, ModPicoGraphics_get_bounds);
MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_set_font_obj, ModPicoGraphics_set_font);
MP_DEFINE_CONST_FUN_OBJ_2(ModPicoGraphics_set_framebuffer_obj, ModPicoGraphics_set_framebuffer);


MP_DEFINE_CONST_FUN_OBJ_1(ModPicoGraphics__del__obj, ModPicoGraphics__del__);


STATIC const mp_rom_map_elem_t ModPicoGraphics_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&ModPicoGraphics_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pen), MP_ROM_PTR(&ModPicoGraphics_set_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_thickness), MP_ROM_PTR(&ModPicoGraphics_set_thickness_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&ModPicoGraphics_clear_obj) },

    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&ModPicoGraphics_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_partial_update), MP_ROM_PTR(&ModPicoGraphics_partial_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_update_speed), MP_ROM_PTR(&ModPicoGraphics_set_update_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_clip), MP_ROM_PTR(&ModPicoGraphics_set_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_clip), MP_ROM_PTR(&ModPicoGraphics_remove_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_span), MP_ROM_PTR(&ModPicoGraphics_pixel_span_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&ModPicoGraphics_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&ModPicoGraphics_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_character), MP_ROM_PTR(&ModPicoGraphics_character_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&ModPicoGraphics_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_measure_text), MP_ROM_PTR(&ModPicoGraphics_measure_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_polygon), MP_ROM_PTR(&ModPicoGraphics_polygon_obj) },
    { MP_ROM_QSTR(MP_QSTR_