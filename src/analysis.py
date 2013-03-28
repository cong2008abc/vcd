#! /bin/python

import os
import struct

mask = 0xF
all_feat = set()
reapt_feat = {}

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
            feat = struct.unpack('Q', pf.read(8))[0]
            #print feat
            #handle_string(feat[0])
            count += 1
            #print feat
            if feat not in all_feat:
                all_feat.add(feat)
            else:
                #reapt_feat.add(feat)
                if feat not in reapt_feat:
                    reapt_feat[feat] = 2
                else:
                    reapt_feat[feat] += 1
        except:
           break
#    print len(reapt_feat)
    return count

def read_file(path):
    count = 0
    for p,d,f in os.walk(path):
        for name in f:
            if name.find('omf') == -1:
                continue
            filename = os.path.join(p, name)
            #print filename
            count += get_all_om_featurea(filename)
    #return ''
    #print 'count',count
    #print len(reapt_feat)
    #print len(all_feat)

    statistics = {}
    statistics[1] = len(all_feat) - len(reapt_feat)
    for item in reapt_feat:
        if reapt_feat[item] not in statistics:
            statistics[reapt_feat[item]] = 1
        else:
            statistics[reapt_feat[item]] += 1
        #total += reapt_feat[item]
        #statistics[reapt_feat[item]].append(item)
    #print 'total', total
    total = len(all_feat)
    for item in statistics:

        #total += statistics[item]
        #if item > 5:
        print item, statistics[item], float(statistics[item]) / total
        #print item, reapt_feat[item]
    print total

if __name__ == '__main__':
    #read_file('/mnt/data/20130326')
    read_file('/mnt/1')
