import struct
import random as r
import sys
import os.path
from datetime import datetime

if len(sys.argv) != 3:
    print(sys.argv[0] + ' N C')
    exit()

N, C = int(sys.argv[1]), int(sys.argv[2])
filename = 'a_' + str(N) + '_' + str(C)

if (os.path.isfile(filename + "_done")):
    print("Data file " + filename + " already exists. Skip creating.")
    exit()

print("To create datafile: " + filename)

f = open(filename, 'wb')
for i in range(N):
    f.write(struct.pack('i', r.randint(1, C)))
f.close()
g = open(filename + "_done", 'wb')
g.write("Created in ")
g.write(str(datetime.now()))
g.close()

print("Datafile: " + filename + " has been created.")
