#! /bin/python

import sys

def ReadFile(path):
	'''read data'''
	pf = open(path)
	for line in pf:
		if len(line) > 34:
			print line[:-1], len(line)
			#print line[:len(line) / 2] 
			#print line[len(line) / 2:]

if __name__ == '__main__':
	ReadFile(sys.argv[1])
