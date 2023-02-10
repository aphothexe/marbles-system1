#include "jpegdec.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_1(JPEG_del_obj, _JPEG_del);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(JPEG_openRAM_obj, _JPEG_openRAM);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(JPEG_openFILE_obj, _JPEG_openFILE);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(JPEG_decode_obj, 1, _JPEG_decode);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(JPEG_getWidth_obj, _JPEG_getWidth);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(JPEG_getHeight_obj, _JPEG_getHeight);

// class
STATIC const mp_rom_map_elem_t JPEG_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&JPEG_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_open_RAM), MP_ROM_PTR(&JPEG_openRAM_obj) },
    { MP_ROM_QSTR(MP_QSTR_open_file), MP_ROM_PTR(&JPEG_openFILE_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&JPEG_decode_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&JPEG_getWidth_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&JPEG_getHeight_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&JPEG_getHeight_obj) },
};

STATIC MP_DEFINE_CONST_DICT(JPEG_locals_dict, JPEG_locals_dict_table);

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    JPEG_type,
    MP_QSTR_jpegdec,
    MP_TYPE_FLAG_NONE,
    make_new, _JPEG_make_new,
    //print, _JPEG_print,
    locals_dict, (mp_obj_dict_t*)&JPEG_locals_dict
);
#else
const mp_obj_type