import os
import sys

def read_log(path, _thres):
    pf = open(path)

    total = 0
    correct = 0
    recall = 0

    up = 0.8
    low = 0.2
    nbin = 9

    interval = (up - low) / nbin
    thres = [low + interval * i  for i in xrange(nbin + 1)]
    nr = [0 for i in xrange(nbin + 1)]
    np = [0 for i in xrange(nbin + 1)]
    #print thres

    ret = {}

    while True:
        line = pf.readline()
        if line == '':
            break
        if line[0] != '>':
            continue

        ans_line = pf.readline()
        while ans_line[0] != '#':
            ans_line = pf.readline()

        #print line.split(' ')

        query = line.split('/')[-1]
        test_num = query.split('.')[0].split('_')[1]
        if test_num == '16':
            continue
        query = query.split('.')[0].split('_')[0]

#        print ans_line
#        if _thres <= simi:
#            recall += 1
#            ans = ans_line.split(' ')[2].strip('/n')
#            if ans != "null":
#                ans = ans.split('/')[-1].split('.')[0]
#                #print ans, query
#                if query == ans:
#                    correct += 1
#                else:
#                    print line[:-1]
#                    print ans_line[:-1]

        simi = float(ans_line.split(' ')[1])
        cor = 0
        ans = ans_line.split(' ')[2].strip('/n')
        if ans != "null":
            ans = ans.split('/')[-1].split('.')[0]
            #print ans, query
            if query == ans:
                cor = 1
        for i in xrange(len(thres)):
            if thres[i] <= simi:
                nr[i] += 1
                np[i] += cor

        total += 1

    for i in xrange(len(thres)):
        print thres[i],
        if nr[i] == 0:
            print 0.0, 0.0
        else:
            print float(np[i]) / nr[i], float(np[i]) / total
    #print float(correct) / recall, float(correct) / total

if __name__ == '__main__':
    path = sys.argv[1]
    thres = float(sys.argv[2])

    read_log(path, thres)
