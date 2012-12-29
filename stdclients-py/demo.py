from blinkentrashclient import Display

import time

d = Display()

delay = 0.1
horizontal = False
accelerating = True
while True:
    if horizontal == False:
        for y in range(0, d.height) + range(d.height-1, -1, -1):
            data = [(255,0,0) if (i//d.width == y) else (0,0,0) for i in xrange(d.width*d.height)]
            d.send(data)
            time.sleep(delay)
    else:
        for x in range(0, d.width) + range(d.width-1, -1, -1):
            data = [(0,255,0) if (i%d.width == x) else (0,0,0) for i in xrange(d.width*d.height)]
            d.send(data)
            time.sleep(delay)
    if accelerating:
        delay *= 0.75
        if delay < 0.01:
            accelerating = False
    else:
        delay *= 1.25
        if delay > 0.1:
            accelerating = True
            horizontal = not horizontal
