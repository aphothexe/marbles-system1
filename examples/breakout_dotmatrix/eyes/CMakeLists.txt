
set(OUTPUT_NAME dotmatrix_eyes)

add_executable(
  ${OUTPUT_NAME}
  eyes.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib breakout_dotmatrix)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})