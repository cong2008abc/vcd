import os
import sys

def read_log(path):
    pf = open(path)
    lines = pf.readlines()
    ret = {}
    i = 0
    key = ''
    while lines[i].find('/mnt') != 0:
        i += 1
    while i < len(lines):
        key = lines[i]
        ret[key] = []
        i += 1
        while i < len(lines) and lines[i].find('/mnt') != 0:
            ret[key].append(lines[i])
            i += 1
        #print ret[key]
    return ret

def get_result(path):
    anw = read_log('answer')
    ret = read_log(path)

    s = 0.0
    recall = 0.0
    c = 0.0
    r_c = 0.0
    for item in ret:
        k = 0
        for r in ret[item]:
            if r in anw[item]:
                k += 1
        if len(ret[item]) != 0:
            p = float(k) / len(ret[item])
            s += p
            c += 1
        if len(anw[item]) != 0:
            recall += float(k) / len(anw[item])
            r_c += 1
        #print item
    print s / c
    print recall / r_c

if __name__ == '__main__':
    get_result(sys.argv[1])
