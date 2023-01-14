#include "drivers/mlx90640/mlx90640.hpp"

#include "micropython/modules/util.hpp"

using namespace pimoroni;

extern "C" {
#include "mlx90640.h"
#include "pimoroni_i2c.h"

typedef 