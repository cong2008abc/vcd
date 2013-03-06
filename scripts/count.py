#! /bin/python

import os
import sys
import heapq

# total num of the tmp file
kFileNum = 0xF
# store the name of the tmp file
tmpfile = set()
filehandle = []
buf = {}

def init(p):
    for i in xrange(0, kFileNum + 1):
        #os.remove(p + 'tmp_' + str(i))
        filehandle.append(open(p + 'tmp_' + str(i), 'w'))
        tmpfile.add(i)
        buf[i] = []

def read_piece_from_sorted(pf):
    '''read a piece data from the sorted file'''
    ret = []
    # read 100 lines of this pf until the EndOfFile
    try:
        for i in xrange(0, 100):
            line = pf.readline()
            if not line:
                break
            line = line.strip().split(' ')
            ret.append([line[0], line[1]])
    except:
        print 'read data error!\n'

    return ret

# multiple merge
def merge_sorted(path):
    '''using a max_heap to order all tmp file'''
    pfs = {}
    for key in tmpfile:
        pfs[key] = open(path + 'sorted_tmp_' + str(key), 'r')

    # read the feature into cache
    key2feature = {}
    for key in tmpfile:
        key2feature[key] = read_piece_from_sorted(pfs[key])

    heap = []

    def push_feature(key):
        # get the first element in each file
        item = key2feature[key][0]
        #key2feature.pop(0)
        feature_count = int(item[1])
        index_val = feature_count * (kFileNum + 1) + key

        # because heap is a min_heap, so
        # index the -val in order to get max val in each pop operator
        heapq.heappush(heap, -index_val)

    # init heap
    heapq.heapify(heap)
    for key in tmpfile:
        push_feature(key)

    # loop until merge finished
    pf_result = open(path + 'result', 'w')
    while True:
        if len(heap) == 0:
            break

        first = heapq.heappop(heap)
        # get the real val
        first = -first
        # get the feature item
        key = first & kFileNum
        item = key2feature[key][0]
        # write this feature to the result file
        pf_result.write(item[0] + ' ' + item[1] + '\n')

        # remove the old feature
        key2feature[key].pop(0)
        # if this cache_of_this_file finish, try to load!
        if len(key2feature[key]) == 0:
            key2feature[key] = read_piece_from_sorted(pfs[key])
            # no more feature, forget it!
            if len(key2feature[key]) == 0:
                continue

        # input new feature into the heap
        push_feature(key)

    # close all file handle
    pf_result.close()

    for key in tmpfile:
        pfs[key].close()

def hash_feature(feature):
    pos = [1, 12, 31]

    key = feature[pos[0]] + feature[pos[1]] + feature[pos[2]]
    h = int(key)

    return h & kFileNum

def sort_tmp(tmp_file, pf):
    tpf = open(tmp_file)
    c_map = {}
    for line in tpf.readlines():
        line = line.rstrip("\n")
        if line not in c_map:
            c_map[line] = 1
        else:
            c_map[line] += 1

    sorted_map = sorted(c_map.items(), key=lambda c_map:c_map[1], reverse=True)
    for item in sorted_map:
        #print item[0][:-1], item[1]
        pf.write(item[0] + ' ' + str(item[1]) + '\n')

def flush_buf(key):
    for item in buf[key]:
        filehandle[key].write(item + "\n")
    print "output buf[", key, "]"
    buf[key] = []

def read_db(db_path):
    pf = open(db_path, 'r')
#    content = pf.read()
#    print content
#    lines = content.split("\n")
    co = 0
    for feature in pf:
        feature = feature.rstrip("\n")
        #print len(feature)
#        co += 1
#        if co % 100000 == 0:
#            print co
        if len(feature) != 32:
            continue
#        key = 0
        key = hash_feature(feature)
        buf[key].append(feature)
        if len(buf[key]) > 1000 * 1000:
            flush_buf(key)

#        filehandle[key].write(feature)
#        if key not in tmpfile:
#            tmpfile.add(key)
#
#        tpf = open(p + 'tmp_' + str(key), 'a')
#        tpf.close()

def out_sort_one_file(file_name):
    path = '.'
    init(path)
    read_db(file_name)
    for k in tmpfile:
        flush_buf(k)

    # sort the tmpfile
    for tmp in tmpfile:
        pf = open(path + 'sorted_tmp_' + str(tmp), 'w')
        sort_tmp(path + 'tmp_' + str(tmp), pf)
        pf.close()

		#print 'Waiting...'
    # merge the tmp sorted file
    merge_sorted(path)

    # remove the tmp file
    for tmp in tmpfile:
        os.remove(path + 'tmp_' + str(tmp))
        os.remove(path + 'sorted_tmp_' + str(tmp))

def out_sort(path):
    init(path)
    # partition the original data into different file
    # by the hash_value
    idx = 0
    for p,d,f in os.walk(path):
        for db in f:
            if db[-3:] == 'omf':
                for f in filehandle:
                    f.flush()
                print "reading", db
                read_db(os.path.join(p,db))
                print 'Finish Partition!', idx
                idx += 1
    for k in tmpfile:
        flush_buf(k)

    # sort the tmpfile
    for tmp in tmpfile:
        pf = open(path + 'sorted_tmp_' + str(tmp), 'w')
        sort_tmp(path + 'tmp_' + str(tmp), pf)
        pf.close()

		#print 'Waiting...'
    # merge the tmp sorted file
    merge_sorted(path)

    # remove the tmp file
    for tmp in tmpfile:
        os.remove(path + 'tmp_' + str(tmp))
        os.remove(path + 'sorted_tmp_' + str(tmp))



if __name__ == '__main__':
    out_sort(sys.argv[1])
    #out_sort_one_file(sys.argv[1])
