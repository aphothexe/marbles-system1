#include "pimoroni_bus.hpp"

namespace pimoroni {
    SPIPins get_spi_pins(BG_SPI_SLOT slot) {
        switch(slot) {
        case PICO_EXPLORER_ONBOARD:
          