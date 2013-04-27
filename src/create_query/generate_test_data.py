import os
import sys
import random

def plus_logo(path):
    direct = ['southeast', 'northeast', 'southwest', 'northwest']

    for p,d,f in os.walk(path):
        for item in f:
            img = os.path.join(p, item)
            num = int(item.split('.')[0])
            nimg = os.path.join('/mnt/share/image_db4', str(num) + '_10.jpg')

            cmd = 'convert ' + img
            cmd += ' logo.png -gravity '

            k = random.randint(0,3)
            cmd += direct[k]

            cmd += ' -geometry +5+10'
            cmd += ' -composite ' + nimg

            print cmd
            os.system(cmd)

if __name__ == '__main__':
    plus_logo(sys.argv[1])
