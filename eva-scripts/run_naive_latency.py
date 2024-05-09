from calendar import c
from multiprocessing.pool import ApplyResult
from numbers import Rational
from operator import index
import queue
from re import A, U
import shutil
import sys
import os


ROOT_PATH = os.getcwd()

ossystem = os.system

SIZE = ["10M", "50M", "100M", "500M", "1B"]

# INDEX PATH
index_path =   { "10M" : "10M_100/", 
                 "50M" : "50M_100", 
                 "100M": "100M_100/", 
                 "500M": "500M_100/",
                 "1B"  : "1B_100/"}

#number_of_rows
number_of_rows = { "10M" : 10000000,
                   "50M" : 50000000,
                   "100M": 100000000,
                   "500M": 500000000,
                   "1B"  : 1000000000}

cardinality = 100

ratio_percentage = 0.1

###################################

def build():
    cmd = 'sh build.sh'
    os.system(cmd)
    
def build_index():
    cmd = 'python3 build_bitvector.py'
    os.system(cmd)
    
####################################
    
def latency_analysis(filename):
    f = open(filename)
    Qvec = [] # vec for operations
    Uvec = []
    Ivec = []
    Dvec = []
    Allvec = []
    ret = []
    for line in f:
        a = line.split()
        if (len(a) != 2):
            continue
        elif line.startswith('Q '):
            Qvec.append(float(a[-1]) / 1000)
        elif line.startswith('U '):
            Uvec.append(float(a[-1]) / 1000)
            Allvec.append(float(a[-1]) / 1000)
        elif line.startswith('I '):
            Ivec.append(float(a[-1]) / 1000)
            Allvec.append(float(a[-1]) / 1000)
        elif line.startswith('D '):
            Dvec.append(float(a[-1]) / 1000)
            Allvec.append(float(a[-1]) / 1000)
        else:
            continue
    if len(Qvec) != 0:
        ret.append(sum(Qvec) / len(Qvec)) 
    else:
        ret.append(0)
    if len(Uvec) != 0:
        ret.append(sum(Uvec) / len(Uvec)) 
    else:
        ret.append(0)
    if len(Ivec) != 0:
        ret.append(sum(Ivec) / len(Ivec)) 
    else:
        ret.append(0)
    if len(Dvec) != 0:
        ret.append(sum(Dvec) / len(Dvec)) 
    else:
        ret.append(0)
    if len(Allvec) != 0:
        ret.append(sum(Allvec) / len(Allvec)) 
    else:
        ret.append(0)
    return ret

def gen_cmd_naive(w, a, c, v, ind_path, n_rows, total, ratio, size): 
    cmd = './build/nicolas -w {} -a {} -m mix --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer no > dat/{}_{}_{}.dat'.format(w, a, int(total * (1 - ratio)), int(total * ratio), c, ind_path, n_rows, v, a, 'latency' if (v == 'yes') else 'throughput', size)
    return cmd
    	   
def gen_naive_latency():
    print ('naive_latency') 
    print ('-' * 10)
    for num in SIZE:
        f = open('dat/figure_naive_latency_{}.dat'.format(num),'w')
        cmd = gen_cmd_naive(1, 'naive', 100, 'yes', index_path[num], number_of_rows[num], 10000, ratio_percentage, num)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat/AVE_naive_latency_{}.dat'.format(num))
        print(ret)
        print('\n')
        for i in range(0, len(ret)):
            f.write('{} '.format(ret[i]))
        f.close()   
    
gen_naive_latency()
