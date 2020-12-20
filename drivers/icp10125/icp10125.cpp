#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>
#include <cstring>

#include "icp10125.hpp"

namespace pimoroni {

    enum command {
        SOFT_RESET = 0x805D,
        READ_ID = 0xEFC8,
        MOVE_ADDRESS_PTR = 0xC595,
        READ_OTP = 0xC7F7
    };
    
#pragma pack(push, 1)
    struct alignas(1) uint16_result {
        uint16_t data;
        uint8_t crc8;
    };
#pragma pack(pop)

    struct conversion_constants {
        float A;
        float B;
        float C;
    };

    bool ICP10125::init() {
        reset();
        uint8_t id = chip_id();
        if(id != CHIP_ID) return false;

        if(!read_otp()) return false;

        return true;
    }

    void ICP10125::reset() {
        uint16_t command = __builtin_bswap16(SOFT_RESET);
        i2c->write_blocking(address, (uint8_t *)&command, 2, false);
        sleep_ms(10); // Soft reset time is 170us but you can never be too sure...
    }

    ICP10125::reading ICP10125::measure(meas_command cmd) {
        uint16_t command = __builtin_bswap16(cmd);
        reading result = {0.0f, 0.0f, OK};
        uint16_result results[3];

        i2c->write_blocking(address, (uint8_t *)&command, 2, false);

        switch(cmd) {
            case NORMAL:
                sleep_ms(7); // 5.6 - 6.3ms
                break;
            case LOW_POWER:
                sleep_ms(2); // 1.6 - 1.8ms
                break;
            case LOW_NOISE:
                sleep_ms(24); // 20.8 - 23.8ms
                break;
            case ULTRA_LOW_NOISE:
                sleep_ms(95); // 83.2 - 94.5ms
                break;
        }

        // Can probably just run this until it succeeds rather than the switch/sleep above.
        // The datasheet implies polling and ignoring NACKs would work.
        i2c->read_blocking(address, (uint8_t *)&results, 9, false);

        if(results[0].crc8 != crc8((uint8_t *)&results[0].data, 2)) {result.status = CRC_FAIL; return result;};
        if(results[1].crc8 != crc8((uint8_t *)&results[1].data, 2)) {result.status = CRC_FAIL; return result;};
        if(results[2].crc8 != crc8((uint8_t *)&results[2].data, 2)) {result.status = CRC_FAIL; return result;};

        int temperature = __builtin_bswap16(results[0].data);
        // Due to all the byte swapping nonsense I'm not sure if I've discarded the LLSB or LMSB here...
        int pressure = ((int32_t)__builtin_bswap16(results[1].data) << 8) | (__builtin_bswap16(results[2].data >> 8)); // LLSB is discarded

        process_data(pressure, temperature, &result.pressure, &result.temperature);
        return result;
    }

    int ICP10125::chip_id() {
        uint16_result result;
        uint16_t command = __builtin_bswap16(READ_ID);

       