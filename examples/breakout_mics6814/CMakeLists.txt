
set(OUTPUT_NAME mics6814_demo)

add_executable(
  ${OUTPUT_NAME}
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib breakout_mics6814)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})