# for output on a `pipeofant 1 4`

import sys

class Display(object):
    def send(self, data):
        assert len(data) == 4*8*9
        sys.stdout.write("".join(chr(x) for x in data))
        sys.stdout.flush()

    width = 4*8
    height = 9
