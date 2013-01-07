#! /bin/python

import sys

def count(path):
	pf = open(path + 'result', 'r')
	_map = {}

	for line in pf:
		item = line.strip().split(' ')
		item[1] = int(item[1])
		if item[1] not in _map:
			_map[item[1]] = 1
		else:
			_map[item[1]] += 1
	
	pf.close()

	pf = open(path + 'count', 'w')
	keys = sorted(_map.keys())
	print keys
	for item in keys:
		pf.write(str(item) + ' ' + str(_map[item]) + '\n')
	pf.close()

if __name__ == '__main__':
	count(sys.argv[1])
