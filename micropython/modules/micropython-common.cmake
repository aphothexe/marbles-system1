include(pimoroni_i2c/micropython)
include(pimoroni_bus/micropython)

include(micropython-common-breakouts)

include(pico_scroll/micropython)
include(pico_rgb_keypad/micropython)
include(pico_unicorn/micropython)
include(pico_wireless/micropython)
include(pico_explorer/micropython)

include(hershey_fonts/micropython)
include(bitmap_fonts/micropython)

include(plasma/micropython)
include(hub75/micropython)
include(pwm/micropython)
include(servo/micropython)
include(encoder/micropython)
include(motor/micropython)
include(qrcode/micropython/micropython)
include(adcfft/micropython)
include(pcf85063a/micropython)

include(picographics/micropython)
include(jpegdec/micropython)
include(galactic_unicorn/micropython)


include(modules_py/modules_py)

# Most board specific ports wont need all of these
copy_module(gfx_pack.py)
copy_module(interstate75.py)
if(PICO_BOARD STREQUAL "pico_w")
    copy_module(automation.py)
    copy_module(inventor.py)
endif()

include(micropython-common-ulab)