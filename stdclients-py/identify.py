from pipeofantclient import Display

d = Display()

ncubes = 1

cube = """
.123321..123321..123321..123321.
1......11......11......11......1
2..XX..22.XXX..22.XXX..22...X..2
3...X..33....X.33....X.33..X...3
4...X..44...X..44.XXX..44.X..X.4
3...X..33..X...33....X.33.XXXX.3
2...X..22.XXXX.22.XXX..22....X.2
1......11......11......11......1
.123321..123321..123321..123321.
"""

lines = "".join(x*ncubes for x in cube.split("\n") if x != '')

table = {'.': 0, '1': 0x01, '2': 0x11, '3': 0x21, '4': 0x31, 'X': 0x31}

d.send([table[c] for c in  lines])

import time
# don't sigpipe the pipeofant
time.sleep(10)
