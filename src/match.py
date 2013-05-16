import sys
import os

def compare(path):
    orig = path.split('/')[-1].split('_')[0]
    orig = '/mnt/share/image_query/' + orig + '.jpg'

    cmd = './match ' + path + ' ' + orig
    os.system(cmd)

def compare_dir(path):
    for p,d,f in os.walk(path):
        for name in f:
            compare(os.path.join(p, name))

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print 'Nothing'
    else:
        compare_dir(sys.argv[1])
