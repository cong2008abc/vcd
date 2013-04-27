import os
import sys
import random

def rand_sample(path):
    pf = open(path)
    lines = pf.readlines()

    up = 5000
    down = len(lines)
    ret = []

    for item in lines:
        k = random.randint(0, down)
        if k < up:
            ret.append(item[:-1])
            up -= 1
        down -= 1

    print len(ret)
    hist = [0 for i in xrange(10)]
    for item in ret:
        num = item.split('/')[-1];

        new_path = os.path.join('/mnt/share/image_db3/', num)
        print item, new_path
        os.rename(item, new_path)


def print_file(path):
    for p,d,f in os.walk(path):
        for item in f:
            print os.path.join(p, item)

if __name__ == '__main__':
    rand_sample('image_name.list')
    #print_file(sys.argv[1])
    #rand_sample(sys.argv[1])
