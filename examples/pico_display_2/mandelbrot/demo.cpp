#define MULTICORE

#include <math.h>
#include "pico/stdlib.h"
#if defined(MULTICORE)
#include "pico/multicore.h"
#endif

#include "libraries/pico_display_2/pico_display_2.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"
#include "button.hpp"


using namespace pimoroni;

// PicoDisplay2 is 320 by 240
#define DISPLAY_WIDTH PicoDisplay2::WIDTH
#define DISPLAY_HEIGHT PicoDisplay2::HEIGHT

ST7789 st7789(DISPLAY_WIDTH, DISPLAY_HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB565 graphics(st7789.width, st7789.height, nullptr);

RGBLED led(PicoDisplay2::LED_R, PicoDisplay2::LED_G, PicoDisplay2::LED_B);

Button button_a(PicoDisplay2::A);
Button button_b(PicoDisplay2::B);
Button button_x(PicoDisplay2::X);
Button button_y(PicoDisplay2::Y);

typedef int32_t fixed_t;

class complex_fixed_t {
public:
    complex_fixed_t() {}
    complex_fixed_t(fixed_t _r, fixed_t _i) : r(_r), i(_i) {}
public:
    fixed_t r;
    fixed_t i;
};
inline bool operator==(const complex_fixed_t& lhs, const complex_fixed_t& rhs){ return lhs.r == rhs.r && lhs.i == rhs.i; }

#define FXD_FRACTIONAL_BITS 25

#define FXD_FROM_INT(x) ((x) << FXD_FRACTIONAL_BITS)
static inline fixed_t float_to_fixed(float x) { return static_cast<fixed_t>(x * static_cast<float>(1u << FXD_FRACTIONAL_BITS)); }
static inline float fixed_to_float(fixed_t x) { return static_cast<float>(x) / static_cast<float>(1u << FXD_FRACTIONAL_BITS); }
static inline fixed_t fixed_multiply(fixed_t a, fixed_t b) {
    const int64_t r = static_cast<int64_t>(a) * static_cast<int64_t>(b);
    return static_cast<int32_t>(r >> FXD_FRACTIONAL_BITS);
}
#define FXD_MUL(x, y) fixed_multiply(x, y)


class MandelbrotView {
public:
    void init(int aSizeX, int aScreenSizeY, uint16_t *aBuffer, int aPalettSize, int aBlockSizeX, int aInterationLimit, int aCore);
    void init(const MandelbrotView& aView, int aBlockSizeX, int aInterationLimit, int aCore);
    void setRange(fixed_t aFxdRangeR, const complex_fixed_t& aFxdCenter);
    void setRange(const MandelbrotView& aView);
    void render(void);
    void createPalettes(int aPaletteSize);
    void nextPalette(void);
    inline void start(void) { running = true; }
    inline void stop(void) { running = false; }
    inline bool isRunning(void) const { return running; }
private:
    bool running;
    int core;
    int screenSizeX;
    int screenSizeY;
    fixed_t fxdRangeR;
    complex_fixed_t fxdCenter;
    complex_fixed_t fxdMin;
    complex_fixed_t fxdMax;
    complex_fixed_t fxdPixel;
    int iterationLimit;
    int blockSizeX;
    uint16_t *pFrameBuffer;
    static const uint8_t PALETTE_COUNT = 6;
    int paletteIndex;
    uint16_t *pPalette;
    uint16_t *pPalettes[PALETTE_COUNT];
};

void MandelbrotView::init(int aScreenSizeX, int aScreenSizeY, uint16_t *aBuffer, int aPaletteSize, int aBlockSizeX, int aInterationLimit, int aCore) {
    screenSizeX = aScreenSizeX;
    screenSizeY = aScreenSizeY;
    createPalettes(aPaletteSize);
    blockSizeX = aBlockSizeX;
    pFrameBuffer = aBuffer;
    iterationLimit = aInterationLimit;
    core = aCore;
    running = true;
}

void MandelbrotView::init(const MandelbrotView& aView, int aBlockSizeX, int aInterationLimit, int aCore) {
    *this = aView;
    for (int ii = 0; ii < PALETTE_COUNT; ++ii) {
        pPalettes[ii] = aView.pPalettes[ii];
    }
    blockSizeX = aBlockSizeX;
    iterationLimit = aInterationLimit;
    core = aCore;
    running = false;
}

void MandelbrotView::setRange(fixed_t aFxdRangeR, const complex_fixed_t& aFxdCenter) {
    fxdRangeR = aFxdRangeR;
    fxdCenter = aFxdCenter;
    fxdMin.r = aFxdCenter.r - fxdRangeR / 2;
    fxdMax.r = aFxdCenter.r + fxdRangeR / 2;
    const float screenRatio2 = static_cast<float>(screenSizeY) / static_cast<float>(screenSizeX * 2);
    const fixed_t fxdRangeI2 = float_to_fixed(fixed_to_float(fxd