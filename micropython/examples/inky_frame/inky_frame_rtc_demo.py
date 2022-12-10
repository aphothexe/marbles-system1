import time
import uasyncio
import WIFI_CONFIG
import inky_frame
from network_manager import NetworkManager
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"

# Sync the Inky (always on) RTC to the Pico W so that "time.localtime()" works.
inky_frame.pcf_to_pico_rtc()

# Avoid running code unless we've been triggered by an event
# Keeps this example from locking up Thonny when we want to tweak the code
if inky_frame.woken_by_rtc() or inky_frame.woken_by_button():
    graphics = PicoGraphics(DISPLAY)
    WIDTH, HEIGHT = graphics.get_bounds()

    graphics.set_pen(1)
    graphics.clear()

    # Look, just because this is an RTC demo,
    # doesn't mean we can't make it rainbow.
    for x in range(WIDTH):
        h = x / WIDTH
        p = graphics.create_pen_hsv(h, 1.0, 1.0)
        graphics.set_pen(p)
        graphics.line(x, 0, x, HEIGHT)

    graphics.set_pen(0)
    graphics.rectangle(0, 0, WIDTH, 14)
    graphics.set_pen(1)
    graphics.text("Inky Frame", 1, 0)
    graphics.set_pen(0)

    def status_handler(mode, status, ip):
        print(mode, status, ip)

    year, month, day, hour, minute, second, dow, _ = time.localtime()

    # Connect to the network and get the time if it's not set
    if year < 2023:
        connected = False
        network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler, client_timeout=60)

        t_start = time.time()
        try:
            uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
            connected = True
        exc