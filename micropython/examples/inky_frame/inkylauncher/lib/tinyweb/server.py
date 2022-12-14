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
    """Decode urlencoded string (including '+' char).
    Returns decoded string
    """
    s = s.replace('+', ' ')
    arr = s.split('%')
    res = arr[0]
    for it in arr[1:]:
        if len(it) >= 2:
            res += chr(int(it[:2], 16)) + it[2:]
        elif len(it) == 0:
            res += '%'
        else:
            res += it
    return res


def parse_query_string(s):
    """Parse urlencoded string into dict.
    Returns dict
    """
    res = {}
    pairs = s.split('&')
    for p in pairs:
        vals = [urldecode_plus(x) for x in p.split('=', 1)]
        if len(vals) == 1:
            res[vals[0]] = ''
        else:
            res[vals[0]] = vals[1]
    return res


class HTTPException(Exception):
    """HTTP protocol exceptions"""

    def __init__(self, code=400):
        self.code = code


class request:
    """HTTP Request class"""

    def __init__(self, _reader):
        self.reader = _reader
        self.headers = {}
        self.method = b''
        self.path = b''
        self.query_string = b''

    async def read_request_line(self):
        """Read and parse first line (AKA HTTP Request Line).
        Function is generator.
        Request line is something like:
        GET /something/script?param1=val1 HTTP/1.1
        """
        while True:
            rl = await self.reader.readline()
            # skip empty lines
            if rl == b'\r\n' or rl == b'\n':
                continue
            break
        rl_frags = rl.split()
        if len(rl_frags) != 3:
            raise HTTPException(400)
        self.method = rl_frags[0]
        url_frags = rl_frags[1].split(b'?', 1)
        self.path = url_frags[0]
        if len(url_frags) > 1:
            self.query_string = url_frags[1]

    async def read_headers(self, save_headers=[]):
        """Read and parse HTTP headers until \r\n\r\n:
        Optional argument 'save_headers' controls which headers to save.
            This is done mostly to deal with memory constrains.
        Function is generator.
        HTTP headers could be like:
        Host: google.com
        Content-Type: blah
        \r\n
        """
        while True:
            gc.collect()
            line = await self.reader.readline()
            if line == b'\r\n':
                break
            frags = line.split(b':', 1)
            if len(frags) != 2:
                raise HTTPException(400)
            if frags[0] in save_headers:
                self.headers[frags[0]] = frags[1].strip()

    async def read_parse_form_data(self):
        """Read HTTP form data (payload), if any.
        Function is generator.
        Returns:
            - dict of key / value pairs
            - None in case of no form data present
        """
        # TODO: Probably there is better solution how to handle
        # request body, at least for simple urlencoded forms - by processing
        # chunks instead of accumulating payload.
        gc.collect()
        if b'Content-Length' not in self.headers:
            return {}
        # Parse payload depending on content type
        if b'Content-Type' not in self.headers:
            # Unknown content type, return unparsed, raw data
            return {}
        size = int(self.headers[b'Content-Length'])
        if size > self.params['max_body_size'] or size < 0:
            raise HTTPException(413)
        data = await self.reader.readexactly(size)
        # Use only string before ';', e.g:
        # application/x-www-form-urlencoded; charset=UTF-8
        ct = self.headers[b'Content-Type'].split(b';', 1)[0]
        try:
            if ct == b'application/json':
                return json.loads(data)
            elif ct == b'application/x-www-form-urlencoded':
                return parse_query_string(data.decode())
        except ValueError:
            # Re-generate exception for malformed form data
            raise HTTPException(400)


class response:
    """HTTP Response class"""

    def __init__(self, _writer):
        self.writer = _writer
        self.send = _writer.awrite
        self.code = 200
        self.version = '1.0'
        self.headers = {}

    async def _send_headers(self):
        """Compose and send:
        - HTTP request line
        - HTTP headers following by \r\n.
        This function is generator.
        P.S.
        Because of usually we have only a few HTTP headers (2-5) it doesn't make sense
        to send them separately - sometimes it could increase latency.
        So combining headers together and send them as single "packet".
        """
        # Request line
        hdrs = 'HTTP/{} {} MSG\r\n'.format(self.version, self.code)
        # Headers
        for k, v in self.headers.items():
            hdrs += '{}: {}\r\n'.format(k, v)
        hdrs += '\r\n'
        # Collect garbage after small mallocs
        gc.collect()
        await self.send(hdrs)

    async def error(self, code, msg=None):
        """Generate HTTP error response
        This function is generator.
        Arguments:
            code - HTTP response code
        Example:
            # Not enough permissions. Send HTTP 403 - Forbidden
            await resp.error(403)
        """
        self.code = code
        if msg:
            self.add_header('Content-Length', len(msg))
        await self._send_headers()
        if msg:
            await self.send(msg)

    async def redirect(self, location, msg=None):
        """Generate HTTP redirect response to 'location'.
        Basically it will generate HTTP 302 with 'Location' header
        Arguments:
            location - URL to redirect to
        Example:
            # Redirect to /something
            await resp.redirect('/something')
        """
        self.code = 302
        self.add_header('Location', location)
        if msg:
            self.add_header('Content-Length', len(msg))
        await self._send_headers()
        if msg:
            await self.send(msg)

    def add_header(self, key, value):
        """Add HTTP response header
        Arguments:
            key - header name
            value - header value
        Example:
            resp.add_header('Content-Encoding', 'gzip')
        """
        self.headers[key] = value

    def add_access_control_headers(self):
        """Add Access Control related HTTP response headers.
        This is required when working with RestApi (JSON requests)
        """
        self.add_header('Access-Control-Allow-Origin', self.params['allowed_access_control_origins'])
        self.add_header('Access-Control-Allow-Methods', self.params['allowed_access_control_methods'])
        self.add_header('Access-Control-Allow-Headers', self.params['allowed_access_control_headers'])

    async def start_html(self):
        """Start response with HTML content type.
        This function is generator.
        Example:
            await resp.start_html()
            await resp.send('<html><h1>Hello, world!</h1></html>')
        """
        self.add_header('Content-Type', 'text/html')
        await self._send_headers()

    async def send_file(self, filename, content_type=None, content_encoding=None, max_age=2592000, buf_size=128):
        """Send local file as HTTP response.
        This function is generator.
        Arguments:
            filename - Name of file which exists in local filesystem
        Keyword arguments:
            content_type - Filetype. By default - None means auto-detect.
            max_age - Cache control. How long browser can keep this file on disk.
                      By default - 30 days
                      Set to 0 - to disable caching.
        Example 1: Default use case:
            await resp.send_file('images/cat.jpg')
        Example 2: Disable caching:
            await resp.send_file('static/index.html', max_age=0)
        Example 3: Override content type:
            await resp.send_file('static/file.bin', content_type='application/octet-stream')
        """
        try:
            # Get file size
            stat = os.stat(filename)
            slen = str(stat[6])
            self.add_header('Content-Length', slen)
            # Find content type
            if content_type:
                self.add_header('Content-Type', content_type)
            # Add content-encoding, if any
            if content_encoding:
                self.add_header('Content-Encoding', content_encoding)
            # Since this is static content is totally make sense
            # to tell browser to cache it, however, you can always
            # override it by setting max_age to zero
            self.add_header('Cache-Control', 'max-age={}, public'.format(max_age))
            with open(filename) as f:
                await self._send_headers()
                gc.collect()
                buf = bytearray(min(stat[6], buf_size))
                while True:
                    size = f.readinto(buf)
                    if size == 0:
                        break
                    await self.send(buf, sz=size)
        except OSError as e:
            # special handling for ENOENT / EACCESS
            if e.args[0] in (errno.ENOENT, errno.EACCES):
                raise HTTPException(404)
            else:
                raise


async def restful_resource_handler(req, resp, param=None):
    """Handler for RESTful API endpoins"""
    # Gather data - query string, JSON in request body...
    data = await req.read_parse_form_data()
    # Add parameters from URI query string as well
    # This one is actually for simply development of RestAPI
    if req.query_string != b'':
        data.update(parse_query_string(req.query_string.decode()))
    # Call actual handler
    _handler, _kwargs = req.params['_callmap'][req.method]
    # Collect garbage before / after handler execution
    gc.collect()
    if param:
        res = _handler(data, param, **_kwargs)
    else:
        res = _handler(data, **_kwargs)
    gc.collect()
    # Handler result could be:
    # 1. generator - in case of large payload
    # 2. string - just string :)
    # 2. dict - meaning client what tinyweb to convert it to JSON
    # it can also return error code together with str / dict
    # res = {'blah': 'blah'}
    # res = {'blah': 'blah'}, 201
    if isinstance(res, type_gen):
        # Result is generator, use chunked response
        # NOTICE: HTTP 1.0 by itself does not support chunked responses, so, making workaround:
        # Response is HTTP/1.1 with Connection: close
        resp.version = '1.1'
        resp.add_header('Connection', 'close')
        resp.add_header('Content-Type', 'application/json')
        resp.add_header('Transfer-Encoding', 'chunked')
        resp.add_access_control_headers()
        await resp._send_headers()
        # Drain generator
        for chunk in res:
            chunk_len = len(chunk.encode('utf-8'))
            await resp.send('{:x}\r\n'.format(chunk_len))
            await resp.send(chunk)
            await resp.send('\r\n')
            gc.collect()
        await resp.send('0\r\n\r\n')
    else:
        if type(res) == tuple:
            resp.code = res[1]
            res = res[0]
        elif res is None:
            raise Exception('Result expected')
        # Send response
        if type(res) is dict:
            res_str = json.dumps(res)
        else:
            res_str = res
        resp.add_header('Content-Type', 'application/json')
        resp.add_header('Content-Length', str(len(res_str)))
        resp.add_access_control_headers()
        await resp._send_headers()
        await resp.send(res_str)


class webserver:

    def __init__(self, request_timeout=3, max_concurrency=3, backlog=16, debug=False):
        """Tiny Web Server class.
        Keyword arguments:
            request_timeout - Time for client to send complete request
                              after that connection will be closed.
            max_concurrency - How many connections can be processed concurrently.
                              It is very important to limit this number because of
                              memory constrain.
                              Default value depends o