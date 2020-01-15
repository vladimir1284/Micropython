#
# This is a picoweb example showing a centralized web page route
# specification (classical Django style).
#
import ure as re
import picoweb
import network

# get the interface's IP/netmask/gw/DNS addresses
ap = network.WLAN(network.AP_IF) # create access-point interface
ap.active(True)
# activate the interface
ap.config(essid='ESP-AP') # set the ESSID of the access point


def index(req, resp):
    # You can construct an HTTP response completely yourself, having
    # a full control of headers sent...
    yield from resp.awrite("HTTP/1.0 200 OK\r\n")
    yield from resp.awrite("Content-Type: text/html\r\n")
    yield from resp.awrite("\r\n")
    yield from resp.awrite("I can show you a table of <a href='squares'>squares</a>.<br/>")
    yield from resp.awrite("Or my <a href='file'>source</a>.")


def squares(req, resp):
    # Or can use a convenience function start_response() (see its source for
    # extra params it takes).
    yield from picoweb.start_response(resp)
    yield from app.render_template(resp, "squares.tpl", (req,))


def hello(req, resp):
    yield from picoweb.start_response(resp)
    # Here's how you extract matched groups from a regex URI match
    yield from resp.awrite("Hello " + req.url_match.group(1))


ROUTES = [
    # You can specify exact URI string matches...
    ("/", index),
    ("/squares", squares),
]


# import ulogging as logging
# logging.basicConfig(level=logging.INFO)
#logging.basicConfig(level=logging.DEBUG)

app = picoweb.WebApp(__name__, ROUTES)
# debug values:
# -1 disable all logging
# 0 (False) normal logging: requests and errors
# 1 (True) debug logging
# 2 extra debug logging
app.run(debug=2)
