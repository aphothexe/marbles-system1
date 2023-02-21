// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

// Declare the functions we'll make available in Python
extern mp_obj_t picowireless_init();
extern mp_obj_t picowireless_get_network_data();
extern mp_obj_t picowireless_get_remote_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_wifi_set_network(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_wifi_set_passphrase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_wifi_set_key(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_set_dns(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_set_hostname(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_disconnect();

extern mp_obj_t picowireless_get_connection_status();
extern mp_obj_t picowireless_get_mac_address();

extern mp_obj_t picowireless_get_ip_address();
extern mp_obj_t picowireless_get_subnet_mask();
extern mp_obj_t picowireless_get_gateway_ip();

extern mp_obj_t picowireless_get_current_s