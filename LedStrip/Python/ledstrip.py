import serial
import re
import logging


class LedStrip:
    def __init__(self, port=None, baudrate=115200, logging_lvl=logging.NOTSET, timeout=1):
        self.ser = serial.Serial(port, baudrate, timeout=timeout)
        self.logger = logging.getLogger('ledstripAPI')
        self.logger.setLevel(logging_lvl)

    def rainbow_with_glitter(self):
        self.ser.write(b'rainbowWithGlitter\n')

    def rainbow(self):
        self.ser.write(b'rainbow\n')

    def all(self, r, g, b):
        req = 'all({0:d},{1:d},{2:d})\n'.format(r, g, b)
        self.ser.write(bytearray(req.encode('utf-8')))

    def any(self, index, r, g, b):
        req = 'any({0:d},{1:d},{2:d},{3:d})\n'.format(index, r, g, b)
        self.ser.write(bytearray(req.encode('utf-8')))

    def one(self, index, r, g, b):
        req = 'one({0:d},{1:d},{2:d},{3:d})\n'.format(index, r, g, b)
        self.ser.write(bytearray(req.encode('utf-8')))

    def sinelon(self):
        self.ser.write(b'sinelon\n')

    def bpm(self):
        self.ser.write(b'bpm\n')

    def get_leds_count(self):
        req = "getLedsCount\n"
        self.ser.write(bytearray(req.encode('utf-8')))
        resp = self.ser.readline().decode("utf-8")
        m = re.search('[0..9]*.', resp)
        if m:
            return int(m.group(0))
        else:
            return 0


if __name__ == '__main__':
    import time
    logging.basicConfig()
    ls = LedStrip('COM16', 115200, logging.DEBUG, 0.1)
    ls.rainbow_with_glitter()
    time.sleep(5)

    ls.sinelon()
    time.sleep(5)

    ls.bpm()
    time.sleep(5)

    for i in range(0, ls.get_leds_count()):
        ls.one(i, 127, 127, 127)
        time.sleep(0.2)
    ls.all(0, 0, 0)
    for i in range(0, ls.get_leds_count()):
        ls.any(i, 15+i*8, 15+i*8, 15+i*8)
        time.sleep(0.2)
    time.sleep(1)
    ls.all(0, 0, 0)
    
