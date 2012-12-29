from pipeofantclient import Display

import time
import random

random.seed(time.time())

d = Display()

width = 4*8
height = 9

pixels = [(x, y) for y in range(height) for x in range(width)]
neightbors = [((x-1)%width, x, (x+1)%width) for x in range(width)]

delay = 0.1

momentums = [0.0] * width
heights = [height / 2.0] * width
heights[width//2] = height

def render():
    d.send([int(0x31 * min(max(0, y - (height-heights[x]) + 1), 1)) for (x, y) in pixels])

def iterate():
    for (left, i, right) in neightbors:
        direction = (heights[left] + heights[right]) / 2.0 - heights[i]
        momentums[i] += direction / 10.0

    for i in range(width):
        heights[i] += momentums[i]

    for i in range(width):
        momentums[i] *= 0.99

    if random.randint(1, 50) == 1:
        momentums[random.randint(0, width-1)] = random.randint(0, height)

while True:
    render()
    iterate()
    time.sleep(delay)
