#! -*-coding:utf-8 -*-
import numpy as np
import pylab as pl
import sys

def read_log(path):
    pf = open(path)

    ret = [[] for i in xrange(3)]
    for line in pf:
        sub = line.strip('/n').split(' ')
        for i in xrange(3):
            ret[i].append(float(sub[i]))

    return ret

def draw_recall_two(da, db):
    pl1, = pl.plot(da[0], da[2], 'r')
    pl2, = pl.plot(db[0], db[2], 'g')

    pl.title('recall of OM vs Improved OM')
    pl.xlabel('threshold')
    pl.ylabel('recall')
    pl.legend([pl1, pl2], ('Improved OM','OM'), 'best')

def draw_recall_three(da, db, dc):
    pl1, = pl.plot(da[0], da[2], 'r')
    pl2, = pl.plot(db[0], db[2], 'g')
    pl3, = pl.plot(dc[0], dc[2], 'b')

    pl.title('recall of OM vs Improved OM vs Saliency OM')
    pl.xlabel('threshold')
    pl.ylabel('recall')
    pl.legend([pl1, pl2, pl3], ('Improved OM','OM', 'Saliency OM'), 'best')

def draw_precision_two(da, db):
    pl1, = pl.plot(da[0], da[1], 'r')
    pl2, = pl.plot(db[0], db[1], 'g')

    pl.title('precision of OM vs Improved OM')
    pl.xlabel('threshold')
    pl.ylabel('precision')
    pl.legend([pl1, pl2], ('Improved OM','OM'), 'best')

def draw_precision_three(da, db, dc):
    pl1, = pl.plot(da[0], da[1], 'r')
    pl2, = pl.plot(db[0], db[1], 'g')
    pl3, = pl.plot(dc[0], dc[1], 'b')

    pl.title('precision of OM vs Improved OM vs Saliency OM')
    pl.xlabel('threshold')
    pl.ylabel('precision')
    pl.legend([pl1, pl2, pl3], ('Improved OM','OM', 'Saliency OM'), 'best')

if __name__ == '__main__':
    da = read_log('imp')
    db = read_log('om')
    dc = read_log('sa')

    if sys.argv[1] == '1':
        draw_recall_two(da, db)
    elif sys.argv[1] == '2':
        draw_precision_two(da, db)
    elif sys.argv[1] == '3':
        draw_recall_three(da, db, dc)
    else:
        draw_precision_three(da, db, dc)

    pl.savefig(sys.argv[2])
