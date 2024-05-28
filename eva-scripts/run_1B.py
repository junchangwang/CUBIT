
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


ONE_HUNDRED_MILLION = 100000000
ONE_BILLION = 1000000000
ONE_MILLION = 1000000

######################################### parameters for ratio and core cmd ##########################################

index_path = "1B_100/" # dataset (-i)

cardinality = 100 # -c

number_of_rows = ONE_BILLION # -n

mode = "mix" # -m

MERGE_THRESHOLD_NBUB = 20

ROWS_PER_SEG = 100000  # 0.1M

NUM_EMBARR_PARALLEL = 2 

PARALLEL_CNT = 'true'

######################################### global parameters ##########################################

ratio_percentage = 0.1

core_for_cardinality = 16


def help_gen_data_and_index(N, C, index_path): # N for number_of_rows; C for cardinality
    cmd = 'python3 generate.py {} {}'
    os.system(cmd.format(N, C))
    cmd2 = './build/nicolas -m build -d {} -c {} -n {} -i {}'
    os.system(cmd2.format("a_{}_{}".format(N, C), C, N, index_path))


###################################### cmd for core and ratio #########################################
def gen_cmd_naive(w, a, v, total, ratio, index_path): 
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer no > dat_tmp_1B/figure_{}_{}_raw_w_{}_ratio_{}.dat'.format(w, a, mode,int(total * (1 - ratio)), int(total * ratio), cardinality, index_path, number_of_rows, v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio )
    return cmd
    
def gen_cmd_cubit(w, a, v, total, ratio, index_path):
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer yes --fence-length 100000 --merge-threshold {} --rows-per-seg {} --num-embarr-parallel {} --parallel-cnt {} > dat_tmp_1B/figure_{}_{}_raw_w_{}_ratio_{}.dat'.format(w, a, mode, int(total * (1 - ratio)), int(total * ratio), cardinality, index_path, number_of_rows, v, MERGE_THRESHOLD_NBUB, ROWS_PER_SEG, NUM_EMBARR_PARALLEL, PARALLEL_CNT, a, 'latency' if (v == 'yes') else 'throughput',w, ratio )
    return cmd

def gen_cmd_others(w, a, v, total, ratio, index_path):
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer yes --fence-length 100000 > dat_tmp_1B/figure_{}_{}_raw_w_{}_ratio_{}.dat'.format(w, a, mode, int(total * (1 - ratio)), int(total * ratio), cardinality, index_path, number_of_rows, v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio )
    return cmd

###################################### cmd for cardinality #########################################
def gen_cmd_naive_cardinality(w, a, v, total, ratio, index_path, c, n): 
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer no > dat_tmp_1B/figure_{}_{}_raw_w_{}_ratio_{}_c_{}.dat'.format(w, a, mode,int(total * (1 - ratio)), int(total * ratio), c, index_path, n, v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio ,c)
    return cmd
    
def gen_cmd_cubit_cardinality(w, a, v, total, ratio, index_path, c, n):
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer yes --fence-length 100000 --merge-threshold {} --rows-per-seg {} --num-embarr-parallel {} --parallel-cnt {} > dat_tmp_1B/figure_{}_{}_raw_w_{}_ratio_{}_c_{}.dat'.format(w, a, mode, int(total * (1 - ratio)), int(total * ratio), c, index_path, n, v, MERGE_THRESHOLD_NBUB, ROWS_PER_SEG, NUM_EMBARR_PARALLEL, PARALLEL_CNT, a, 'latency' if (v == 'yes') else 'throughput',w, ratio ,c)
    return cmd

def gen_cmd_others_cardinality(w, a, v, total, ratio, index_path, c, n):
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer yes --fence-length 100000 > dat_tmp_1B/figure_{}_{}_raw_w_{}_ratio_{}_c_{}.dat'.format(w, a, mode, int(total * (1 - ratio)), int(total * ratio), c, index_path, n, v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio ,c)
    return cmd



def throughput_analysis(filename):
    f = open(filename)
    ret = 0.0
    for line in f:
        a = line.split()
        if len(a) != 3 or a[0] != 'Throughput':
            continue
        ret += float(a[1])
    return ret

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
            Qvec.append(float(a[-1]) / 1000000)
        elif line.startswith('U '):
            Uvec.append(float(a[-1]) / 1000000)
            Allvec.append(float(a[-1]) / 1000000)
        elif line.startswith('I '):
            Ivec.append(float(a[-1]) / 1000000)
            Allvec.append(float(a[-1]) / 1000000)
        elif line.startswith('D '):
            Dvec.append(float(a[-1]) / 1000000)
            Allvec.append(float(a[-1]) / 1000000)
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




############################################## NAIVE ############################################################
# naive throughput, core
def gen_figure_naive_throughput_core(): 
    print ('Figure naive_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_naive_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_naive(num, 'naive', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_naive_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#naive throughput, ratio
def gen_figure_naive_throughput_ratio():
    print ('Figure naive_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_naive_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_naive(16, 'naive', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_naive_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# naive throughput, cardinality
def gen_figure_naive_throughput_cardinality(): 
    print ('Figure naive_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_naive_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_naive_cardinality(core_for_cardinality, 'naive', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_naive_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#naive latency, core
def gen_figure_naive_latency_core(): # latency
    print ('Figure naive_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_naive_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_naive(num, 'naive', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_naive_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

#naive latency, ratio
def gen_figure_naive_latency_ratio():
    print ('Figure naive_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_naive_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_naive(16, 'naive', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_naive_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

# naive latency, cardinality
def gen_figure_naive_latency_cardinality(): 
    print ('Figure naive_latency_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_naive_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_naive_cardinality(core_for_cardinality, 'naive', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_naive_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

############################################## UCB ############################################################

# ucb throughput, core
def gen_figure_ucb_throughput_core(): 
    print ('Figure ucb_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_ucb_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ucb', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_ucb_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#ucb throughput, ratio
def gen_figure_ucb_throughput_ratio():
    print ('Figure ucb_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_ucb_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ucb', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_ucb_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# ucb throughput, cardinality
def gen_figure_ucb_throughput_cardinality(): 
    print ('Figure ucb_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_ucb_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'ucb', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_ucb_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#ucb latency, core
def gen_figure_ucb_latency_core(): # latency
    print ('Figure ucb_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_ucb_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ucb', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_ucb_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

#ucb latency, ratio
def gen_figure_ucb_latency_ratio():
    print ('Figure ucb_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_ucb_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ucb', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_ucb_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

# ucb latency, cardinality
def gen_figure_ucb_latency_cardinality(): 
    print ('Figure ucb_latency_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_ucb_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'ucb', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_ucb_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

############################################## UB ##########################################################
# ub throughput, core
def gen_figure_ub_throughput_core(): 
    print ('Figure ub_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_ub_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ub', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_ub_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#ub throughput, ratio
def gen_figure_ub_throughput_ratio():
    print ('Figure ub_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_ub_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ub', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_ub_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# ub throughput, cardinality
def gen_figure_ub_throughput_cardinality(): 
    print ('Figure ub_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_ub_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'ub', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_ub_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#ub latency, core
def gen_figure_ub_latency_core(): # latency
    print ('Figure ub_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_ub_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ub', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_ub_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

#ub latency, ratio
def gen_figure_ub_latency_ratio():
    print ('Figure ub_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_ub_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ub', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_ub_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

# ub latency, cardinality
def gen_figure_ub_latency_cardinality(): 
    print ('Figure ub_latency_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_ub_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'ub', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_ub_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

############################################## NBUB-lk ##########################################################
def gen_figure_cubit_lk_throughput_core(): 
    print ('Figure cubit-lk_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_cubit-lk_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_cubit(num, 'cubit-lk', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_cubit-lk_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#cubit-lk throughput, ratio
def gen_figure_cubit_lk_throughput_ratio():
    print ('Figure cubit-lk_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_cubit-lk_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_cubit(16, 'cubit-lk', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_cubit-lk_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# cubit-lk throughput, cardinality
def gen_figure_cubit_lk_throughput_cardinality(): 
    print ('Figure cubit-lk_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_cubit-lk_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_cubit_cardinality(core_for_cardinality, 'cubit-lk', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_cubit-lk_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#cubit-lk latency, core
def gen_figure_cubit_lk_latency_core(): # latency
    print ('Figure cubit-lk_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_cubit-lk_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_cubit(num, 'cubit-lk', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_cubit-lk_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

#cubit-lk latency, ratio
def gen_figure_cubit_lk_latency_ratio():
    print ('Figure cubit-lk_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_cubit-lk_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_cubit(16, 'cubit-lk', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_cubit-lk_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

# cubit-lk latency, cardinality
def gen_figure_cubit_lk_latency_cardinality(): 
    print ('Figure cubit-lk_latency_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_cubit-lk_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_cubit_cardinality(core_for_cardinality, 'cubit-lk', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_cubit-lk_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

############################################## NBUB_LF ##########################################################
def gen_figure_cubit_lf_throughput_core(): 
    print ('Figure cubit-lf_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_cubit-lf_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_cubit(num, 'cubit-lf', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_cubit-lf_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#cubit_lf throughput, ratio
def gen_figure_cubit_lf_throughput_ratio():
    print ('Figure cubit-lf_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_cubit-lf_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_cubit(16, 'cubit-lf', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_cubit-lf_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# cubit-lf throughput, cardinality
def gen_figure_cubit_lf_throughput_cardinality(): 
    print ('Figure cubit-lf_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_cubit-lf_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_cubit_cardinality(core_for_cardinality, 'cubit-lf', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp_1B/figure_cubit-lf_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#cubit_lf latency, core
def gen_figure_cubit_lf_latency_core(): # latency
    print ('Figure cubit-lf_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat_1B/figure_cubit-lf_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_cubit(num, 'cubit-lf', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_cubit-lf_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

#cubit_lf latency, ratio
def gen_figure_cubit_lf_latency_ratio():
    print ('Figure cubit-lf_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat_1B/figure_cubit-lf_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_cubit(16, 'cubit-lf', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_cubit-lf_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

# cubit-lf latency, cardinality
def gen_figure_cubit_lf_latency_cardinality(): 
    print ('Figure cubit-lf_latency_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat_1B/figure_cubit-lf_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_cubit_cardinality(core_for_cardinality, 'cubit-lf', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp_1B/figure_cubit-lf_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

def run():
    # #naive
    gen_figure_naive_throughput_core()
    gen_figure_naive_throughput_ratio()
    gen_figure_naive_throughput_cardinality()
    gen_figure_naive_latency_core()
    gen_figure_naive_latency_ratio()
    gen_figure_naive_latency_cardinality()

    # #ucb
    gen_figure_ucb_throughput_core()
    gen_figure_ucb_throughput_ratio()
    gen_figure_ucb_throughput_cardinality()
    gen_figure_ucb_latency_core()
    gen_figure_ucb_latency_ratio()
    gen_figure_ucb_latency_cardinality()

    # #ub
    gen_figure_ub_throughput_core()
    gen_figure_ub_throughput_ratio()
    gen_figure_ub_throughput_cardinality()
    gen_figure_ub_latency_core()
    gen_figure_ub_latency_ratio()
    gen_figure_ub_latency_cardinality()

    # #cubit-lk
    gen_figure_cubit_lk_throughput_core()
    gen_figure_cubit_lk_throughput_ratio()
    gen_figure_cubit_lk_throughput_cardinality()
    gen_figure_cubit_lk_latency_core()
    gen_figure_cubit_lk_latency_ratio()
    gen_figure_cubit_lk_latency_cardinality()

    # #cubit_lf
    gen_figure_cubit_lf_throughput_core()
    gen_figure_cubit_lf_throughput_ratio()
    gen_figure_cubit_lf_throughput_cardinality()
    gen_figure_cubit_lf_latency_core()
    gen_figure_cubit_lf_latency_ratio()
    gen_figure_cubit_lf_latency_cardinality()

def main():
    itr = 0
    cmd = 'mv graphs_1B graphs_1B_{}'
    cmd2 = "mv dat_1B graphs_1B"
    cmd4 = "mv dat_tmp_1B graphs_1B"
    while itr < 2:
        os.chdir(ROOT_PATH)

        # generate 1B
        if (os.path.isfile("1B_100")):
            print("1B_100 exists")
        else:
            help_gen_data_and_index(number_of_rows, cardinality, index_path)
        datdir = 'dat_1B'
        if os.path.exists(datdir) and os.path.isdir(datdir):
            print ('Deleting existing directory ./dat_1B')
            shutil.rmtree(datdir)
        os.mkdir(datdir)

        dat_tmp_1B = 'dat_tmp_1B'
        if os.path.exists(dat_tmp_1B) and os.path.isdir(dat_tmp_1B):
            print ('Deleting existing directory ./dat_tmp_1B')
            shutil.rmtree(dat_tmp_1B)
        os.mkdir(dat_tmp_1B)

        graphs = 'graphs_1B'
        if os.path.exists(graphs) and os.path.isdir(graphs):
            print ('Deleting existing directory ./graphs_1B')
            shutil.rmtree(graphs)
        os.mkdir(graphs)

        run()
        os.system(cmd4)
        os.system(cmd2)       
        os.system(cmd.format(itr))
        itr += 1
    print('Done!\n')

if __name__ == '__main__':
    main()
