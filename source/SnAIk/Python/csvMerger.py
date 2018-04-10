import glob
import csv
import os

interesting_files = glob.glob("*/*.csv")
interesting_files = sorted( interesting_files, key = lambda file: os.path.getctime(file))
with open('output.csv','w') as fout:
    h = True
    for filename in interesting_files:
        with open(filename,'r') as fin:
            if h:
                h = False
            else:
                try:
                    next(fin)
                except:
                    pass
            for line in fin:
                if line != '\n':
                    fout.write(line)
print('DONE!')