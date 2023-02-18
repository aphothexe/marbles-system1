#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "libraries/pico_wireless/pico_wireless.hpp"

using namespace pimoroni;

PicoWireless *wireless = nullptr;


extern "C" {
#include "pico_wireless.h"

#define NOT_INITIALISED_MSG     "Cannot call this function, as picowireless is not initialised. Call picowireless.init() first."

static void mp_obj_to_string(const mp_obj_t &obj, std::string &string_out) {
    if(mp_obj_is_str_or_bytes(obj)) {
        GET_STR_DATA_LEN(obj, str, str_len);
        string_out = std::string((const char*)str);
    }
    else if(mp_obj_is_float(obj))
        mp_raise_TypeError("can't convert 'float' object to str implicitly");
    else if(mp_obj_is_int(obj))
        mp_raise_TypeError("can't convert 'int' object to str implicitly");
    else if(mp_obj_is_bool(obj))
        mp_raise_TypeError("can't convert 'bool' object to str implicitly");
    else
        mp_raise_TypeError("can't convert object to str implicitly");
}

uint32_t mp_obj_to_ip(mp_obj_t obj) {
    uint len;
    mp_obj_t *items;
    mp_obj_tuple_get(obj, &len, &items);
    uint a = mp_obj_get_int(items[0]) & 0xff;
    uint b = mp_obj_get_int(items[1]) & 0xff;
    uint c = mp_obj_get_int(items[2]) & 0xff;
    uint d = mp_obj_get_int(items[3]) & 0xff;
    return (d << 24) | (c << 16) | (b << 8) | a;
}

mp_obj_t mp_ip_to_obj(IPAddress ip) {
    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int(ip[0]);
    tuple[1] = mp_obj_new_int(ip[1]);
    tuple[2] = mp_obj_new_int(ip[2]);
    tuple[3] = mp_obj_new_int(ip[3]);
    return mp_obj_new_tuple(4, tuple);
}

mp_obj_t picowireless_init() {
    if(wireless == nullptr)
        wireless = new PicoWireless();
    wireless->init();
    return mp_const_none;
}

mp_obj_t picowireless_get_network_data() {
    if(wireless != nullptr) {
        uint8_t *ip = nullptr;
        uint8_t *mask = nullptr;
        uint8_t *gwip = nullptr;
        wireless->get_network_data(ip, mask, gwip);

        mp_obj_t tuple[3];
        tuple[0] = mp_obj_new_bytes(ip, WL_IPV4_LENGTH);
        tuple[1] = mp_obj_new_bytes(mask, WL_IPV4_LENGTH);
        tuple[2] = mp_obj_new_bytes(gwip, WL_IPV4_LENGTH);
        return mp_obj_new_tuple(3, tuple);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_remote_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t *ip = nullptr;
        uint8_t *port = nullptr;
        wireless->get_remote_data(args[ARG_sock].u_int, ip, port);

        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_bytes(ip, WL_IPV4_LENGTH);
        tuple[1] = mp_obj_new_int((uint16_t)port[0] << 8 | (uint16_t)port[1]); //TODO verify size and ordering of port
        return mp_obj_new_tuple(2, tuple);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_wifi_set_network(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_ssid, ARG_passphrase };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        std::string ssid;
        mp_obj_to_string(args[ARG_ssid].u_obj, ssid);
        return mp_obj_new_int(wireless->wifi_set_network(ssid));
    }
    else
        mp_rais