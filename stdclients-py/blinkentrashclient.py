# for output on a `blinkentrash 40 10` when called as ./blinkentrashclient.py 40 10

import sys


class Display(object):
    def send(self, data):
        assert len(data) == self.width * self.height
        string = "".join(chr(r)+chr(g)+chr(b) for (r,g,b) in data)
        assert len(string) == self.width * self.height * 3
        sys.stdout.write(string)
        sys.stdout.flush()

    width = int(sys.argv[1])
    height = int(sys.argv[2])
