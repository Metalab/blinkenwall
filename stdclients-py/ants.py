import array

class Field(object):
    def __init__(self, x, y):
        self.total_x = x
        self.total_y = y
        self.values = array.array('B', [0]*x*y)

        self.transitions = {0: 0x31, 0x31: 0}

    def xy2index(self, x, y):
        return x + self.total_x * y

    def get(self, x, y):
        return self.values[self.xy2index(x, y)]

    def set(self, x, y, value):
        self.values[self.xy2index(x, y)] = value

    def increment(self, x, y):
        self.set(x, y, self.transitions[self.get(x, y)])

    def get_next(self, x, y, direction):
        delta = {0: -1, 1: -1, 2: 1, 3: 1}[direction]
        if direction % 2 == 0:
            x = (x + delta) % self.total_x
        else:
            y = (y + delta) % self.total_y

        return (x, y)

class Walker(object):
    def __init__(self, x, y, direction):
        self.x = x
        self.y = y
        self.direction = direction

        self.rule = {0: 1, 0x31: -1}

    def walk(self, field):
        oldvalue = field.get(self.x, self.y)
        self.direction = (self.direction + self.rule[oldvalue]) % 4
        field.increment(self.x, self.y)
        self.x, self.y = field.get_next(self.x, self.y, self.direction)

    def __repr__(self):
        return '<Walker at (%d, %d) to %d>'%(self.x, self.y, self.direction)

def ascii_main():
    f = Field(10, 10)
    w = Walker(5, 5, 0)

    for i in range(30):
        for y in range(10):
            for x in range(10):
                print f.get(x, y),
            print
        print
        print w
        w.walk(f)
        print w

def cube_main():
    from pipeofantclient import Display
    import time

    f = Field(4*8, 9)
    w = Walker(3, 3, 0)

    d = Display()

    while True:
        time.sleep(0.1)
        w.walk(f)
        d.send(f.values)

if __name__ == "__main__":
    cube_main()
