"""
Tiny Web - pretty simple and powerful web server for tiny platforms like ESP8266 / ESP32
MIT license
(C) Konstantin Belyalov 2017-2018
"""
import logging
import uasyncio as asyncio
import uasyncio.core
import ujson as json
import gc
import uos as os
import sys
import uerrno as errno
import usocket as socket


log = logging.getLogger('WEB')

type_gen = type((lambda: (yield))())  # noqa: E275

# uasyncio v3 is shipped with MicroPython 1.13, and contains some subtle
# but breaking changes. See also https://github.com/peterhinch/micropython-async/blob/master/v3/README.md
IS_UASYNCIO_V3 = hasattr(asyncio, "__version__") and asyncio.__version__ >= (3,)


def urldecode_plus(s):
    """Decode urlencoded string (incl