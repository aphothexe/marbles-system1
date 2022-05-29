
add_executable(
  wireless_rgb_http
  rgb_http.cpp
)

# enable usb output, enable uart output
pico_enable_stdio_usb(wireless_rgb_http 1)
pico_enable_stdio_uart(wireless_rgb_http 1)

# Pull in pico libraries that we need
target_link_libraries(wireless_rgb_http pico_stdlib pico_wireless)

# create map/bin/hex file etc.
pico_add_extra_outputs(wireless_rgb_http)