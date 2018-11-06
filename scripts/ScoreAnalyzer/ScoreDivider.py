
"""
   Score File Divider
"""

import argparse
import re

argp = argparse.ArgumentParser(description="Score File Divider")
argp.add_argument("ifile", metavar = 'InputPath',type = str)
argp.add_argument("odir" , metavar = 'OutputDir',type = str, default = '.', nargs='?')
args = argp.parse_args()

output_file = args.odir + '/data_{0}_{1}.txt'
output_file_count = 1;

of = None
oldnum = 0

with open(args.ifile, mode="r") as inf:
	for l in inf:
		if re.match("START", l):
			num = int(re.match(r"START\s*\((\d+)\)", l).group(1));
			if num < oldnum:
				output_file_count+=1
			oldnum = num
			if of:
				of.close()
			of = open(output_file.format(output_file_count, num), mode="w")
		of.write(l)
if of:
	of.close()


