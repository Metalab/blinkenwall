# for output on a `blinkentrash 40 10`

import sys

class Display(object):
    def send(self, data):
        assert len(data) == 40 * 10
        string = "".join(chr(r)+chr(g)+chr(b) for (r,g,b) in data)
        assert len(string) == 40 * 10 * 3
        sys.stdout.write(string)
        sys.stdout.flush()

    width = 40
    height = 10
