// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico W"

// todo: We need something to check our binary size
#define MICROPY_HW_FLASH_STORAGE_BYTES          (848 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK 1

// CYW43 driver configuration.
#define CYW43_USE_SPI (1)
#defin