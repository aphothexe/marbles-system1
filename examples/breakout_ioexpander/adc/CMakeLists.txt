
set(OUTPUT_NAME ioexpander_adc)

add_executable(
  ${OUTPUT_NAME}
  adc.cpp
)

# enable usb output, disable uart output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)
pico_enable_stdio_uart(${OUTPUT_NAME} 1)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib breakout_ioexpander)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})