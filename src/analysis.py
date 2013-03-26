#! /bin/python

import os
import struct

mask = 0xF
all_feat = set()
reapt_feat = set()

def uncompress_feature(data):
    ret = ''
    for i in xrange(0, 16):
        f = data & mask
        ret += str(f)
        data = data >> 4
    return ret
#    if ret in all_feat:
#        reapt_feat.add(ret)
#    else:
#        all_feat.add(ret)
    #f1 = ( >> 4) & mask

def get_all_om_featurea(filename):
    pf = open(filename, 'rb')
    count = 0
    while True:
        try:
            feat = struct.unpack('Q', pf.read(8)) 
            #print feat
            #handle_string(feat[0])
            count += 1
            if feat not in all_feat:
                all_feat.add(feat)
            else:
                reapt_feat.add(feat)
        except:
           break
    print len(reapt_feat)
    return count

def read_file(path):
    count = 0
    for p,d,f in os.walk(path):
        for name in f:
            if name.find('omf') == -1:
                continue
            filename = os.path.join(p, name)
            print filename
            count += get_all_om_featurea(filename)
    print count

if __name__ == '__main__':
    #read_file('/mnt/data/20130326')
    read_file('/mnt/data2/20130322')
