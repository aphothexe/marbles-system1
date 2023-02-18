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
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_wifi_set_passphrase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_ssid, ARG_passphrase };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_passphrase, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        std::string ssid, passphrase;
        mp_obj_to_string(args[ARG_ssid].u_obj, ssid);
        mp_obj_to_string(args[ARG_passphrase].u_obj, passphrase);
        return mp_obj_new_int(wireless->wifi_set_passphrase(ssid, passphrase));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_wifi_set_key(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_ssid, ARG_key_idx, ARG_passphrase };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_key_idx, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_passphrase, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t key_idx = args[ARG_key_idx].u_int;

        std::string ssid, passphrase;
        mp_obj_to_string(args[ARG_ssid].u_obj, ssid);
        mp_obj_to_string(args[ARG_passphrase].u_obj, passphrase);
        return mp_obj_new_int(wireless->wifi_set_key(ssid, key_idx, passphrase));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_valid_params, ARG_local_ip, ARG_gateway, ARG_subnet };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_valid_params, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_local_ip, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_gateway, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_subnet, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t valid_params = args[ARG_valid_params].u_int;
        uint32_t local_ip = mp_obj_to_ip(args[ARG_local_ip].u_obj);
        uint32_t gateway = mp_obj_to_ip(args[ARG_gateway].u_obj);
        uint32_t subnet = mp_obj_to_ip(args[ARG_subnet].u_obj);
        wireless->config(valid_params, local_ip, gateway, subnet);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_set_dns(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_dns_server1, ARG_dns_server2 };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_dns_server1, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_dns_server2 | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t valid_params = n_args;
        uint32_t dns_server1 = mp_obj_to_ip(args[ARG_dns_server1].u_obj);
        uint32_t dns_server2 = n_args == 1 ? 0 : mp_obj_to_ip(args[ARG_dns_server2].u_obj);
        wireless->set_dns(valid_params, dns_server1, dns_server2);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_set_hostname(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_hostname };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_hostname, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        std::string hostname;
        mp_obj_to_string(args[ARG_hostname].u_obj, hostname);
        wireless->set_hostname(hostname);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_disconnect() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->disconnect());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_connection_status() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_connection_status());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_mac_address() {
    if(wireless != nullptr) {
        uint8_t* mac = wireless->get_mac_address();
        return mp_obj_new_bytes(mac, WL_MAC_ADDR_LENGTH);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_ip_address() {
    if(wireless != nullptr) {
        IPAddress ip;
        wireless->get_ip_address(ip);
        return mp_ip_to_obj(ip);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_subnet_mask() {
    if(wireless != nullptr) {
        IPAddress mask;
        wireless->get_subnet_mask(mask);
        return mp_ip_to_obj(mask);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_gateway_ip() {
    if(wireless != nullptr) {
        IPAddress mask;
        wireless->get_gateway_ip(mask);
        return mp_ip_to_obj(mask);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_ssid() {
    if(wireless != nullptr) {
        std::string ssid = wireless->get_current_ssid();
        return mp_obj_new_str(ssid.c_str(), ssid.length());
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_bssid() {
    if(wireless != nullptr) {
        uint8_t* bssid = wireless->get_current_bssid();
        return mp_obj_new_bytes(bssid, WL_MAC_ADDR_LENGTH);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_rssi() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_current_rssi());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_encryption_type() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_current_encryption_type());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);    
    
    return mp_const_none;
}

mp_obj_t picowireless_start_scan_networks() {
    // This doesn't actually *do* anything, so might as well save a few instructions!
    
    return mp_const_true;
}

mp_obj_t picowireless_get_scan_networks() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_scan_networks());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);    
    
    return mp_const_none;
}

mp_obj_t picowireless_get_ssid_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_network_item };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_network_i