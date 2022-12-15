from urllib import urequest
import gc
import qrcode

# Uncomment one URL to use (Top Stories, World News and technology)
# URL = "http://feeds.bbci.co.uk/news/rss.xml"
# URL = "http://feeds.bbci.co.uk/news/world/rss.xml"
URL = "http://feeds.bbci.co.uk/news/technology/rss.xml"

# Length of time between updates in minutes.
# Frequent updates will reduce battery life!
UPDATE_INTERVAL = 90

graphics = None
WIDTH = None
HEIGHT = None
code = qrcode.QRCode()


def read_until(stream, char):
    result = b""
    while True:
        c = stream.read(1)
        if c == char:
            return result
        result += c


def discard_until(stream, c):
    while stream.read(1) != c:
        pass


def parse_xml_stream(s, accept_tags, group_by, max_items=3):
    tag = []
    text = b""
    count = 0
    current = {}
    while True:
        char = s.read(1)
        if len(char) == 0:
            break

        if char == b"<":
            next_char = s.read(1)

            # Discard stuff like <?xml vers...
            if next_char == b"?":
                discard_until(s, b">")
                continue

            # Detect <![CDATA
            elif next_char == b"!":
                s.read(1)  # Discard [
                discard_until(s, b"[")  # Discard CDATA[
                text = read_until(s, b"]")
                discard_until(s, b">")  # Discard ]>
                gc.collect()

            elif next_char == b"/":
                current_tag = read_until(s, b">")
                top_tag = tag[-1]

                # Populate our result dict
                if top_tag in accept_tags:
                    current[top_tag.decode("utf-8")] = text.decode("utf-8")

                # If we've found a group of items, yield the dict
                elif top_tag == group_by:
                    yield current
                    current = {}
                    count += 1
                    if count == max_items:
                        return
                tag.pop()
                text = b""
                gc.collect()
                continue

            else:
                current_tag = read_until(s, b">")
                tag += [next_char + current_tag.split(b" ")[0]]
                text = b""
                gc.collect()

        else:
            text += char


def measure_qr_code(size, code):
    w, h = code.get_size()
    module_size = int(size / w)
    return module_size * w, module_size


def