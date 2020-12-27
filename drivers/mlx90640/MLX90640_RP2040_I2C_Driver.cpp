#include "src/headers/MLX90640_I2C_Driver.h"
#include "mlx90640.hpp"

#include "stdio.h"


static pimoroni::I2C *i2c;

void MLX90640_I2CConfigure(pimoroni::I2C *i2c_instance) {
    i2c = i2c_instance;
}

void MLX90640_I2CInit()
{
    // i2c->init();  // Called in constructor
}

int MLX90640_I2CGeneralReset(void)
{
    return 0;
}

int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *d