
"""
   Plot Score
"""

import argparse
import numpy as np
import matplotlib.colors as colors
import matplotlib.pyplot as pl
import re

argp = argparse.ArgumentParser(description="Plot Score File")
argp.add_argument("ifile", metavar = 'InputPath',type = str)
argp.add_argument("-r", metavar = 'range',type = int , nargs=2, default = (0, 0))
args = argp.parse_args()

data = np.genfromtxt(args.ifile, delimiter =',', skip_header = 10, names=True)

names = data.dtype.names
if data[names[-1]][0] != data[names[-1]][0]:
	names = names[:-1]

prange=[args.r[0],args.r[1]]
if(prange[0] < 0):
	prange[0] += len(data[names[0]])
if(prange[1] <= 0):
	prange[1] += len(data[names[0]])

pos = {}
method = {}

def GetColor(id):
	global pos
	try:
		return pos[id[0:2]]
	except:
		cmap=pl.get_cmap('tab20')
		pos[id[0:2]]=cmap(len(pos))
		return pos[id[0:2]]

def GetLine(id):
	try:
		return method[id[2:]]
	except:
		method_type = ('-', '--', '-.', ':');
		method[id[2:]] = method_type[len(method) % 4]
		return method[id[2:]]

for n in names:
	color = GetColor(n)
	line  = GetLine(n)
	pl.plot(range(prange[0],prange[1]),data[n][prange[0]:prange[1]],color=color,linestyle=line)
	
pl.legend(names)
pl.show()

