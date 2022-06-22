set(OUTPUT_NAME servo2040_servo_wave)
add_executable(${OUTPUT_NAME} servo2040_servo_wave.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
 