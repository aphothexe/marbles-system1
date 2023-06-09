
add_executable(
  wireless_networks
  wifi_networks.cpp
)

# enable usb output, disable uart output
pico_enable_stdio_usb(wireless_networks 1)
pico_enable_stdio_uart(wireless_networks 1)

# Pull in pico libraries that we need
target_link_libraries(wireless_networks pico_stdlib pico_wireless)

# create map/bin/hex file etc.
pico_add_extra_outputs(wireless_networks)