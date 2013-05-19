
import sys

def read_log(path):
    pf = open(path)
    for line in pf:
        if line.find('hash') == 0:
            #print line[:-1]
            num = line.strip().split(':')[1].split('[')[0]
            #print num
            if int(num) > 2000:
                print line.strip(), num

if __name__ == '__main__':
    read_log(sys.argv[1])
