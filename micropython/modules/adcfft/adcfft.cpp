#include "libraries/adcfft/adcfft.hpp"
#include "micropython/modules/util.hpp"

extern "C" {
#include "adcfft.h"

typedef struct _adcfft_obj_t {
    mp_obj_base_t base;
    ADCFFT *adcfft;
} adcfft_obj_t;

mp_obj_t adcfft_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_adc_channel, ARG_adc_gpio, ARG_sample_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_adc_channel, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_adc_gpio, MP_ARG_INT, {.u_int = 