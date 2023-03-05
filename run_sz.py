
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

def build():
    cmd = 'sh build-light.sh'
    os.system(cmd)

ONE_HUNDRED_MILLION = 100000000
ONE_BILLION = 1000000000
ONE_MILLION = 1000000

######################################### parameters for ratio and core cmd ##########################################

index_path = "100M_100/" # dataset (-i)

cardinality = 100 # -c

number_of_rows = ONE_HUNDRED_MILLION # -n

mode = "mix" # -m

######################################### global parameters ##########################################

ratio_percentage = 0.1

core_for_cardinality = 32


def help_gen_data_and_index(N, C, index_path): # N for number_of_rows; C for cardinality
    cmd = 'python generate.py {} {}'
    os.system(cmd.format(N, C))
    cmd2 = './build/nicolas -m build -d {} -c {} -n {} -i {}'
    os.system(cmd2.format("a_{}_{}".format(N, C), C, N, index_path))


###################################### cmd for core and ratio #########################################
def gen_cmd_naive(w, a, v, total, ratio, index_path): 
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer no > dat_tmp/figure_{}_{}_raw_w_{}_ratio_{}.dat'.format(w, a, mode,int(total * (1 - ratio)), int(total * ratio), cardinality, index_path, number_of_rows, v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio )
    return cmd

def gen_cmd_others(w, a, v, total, ratio, index_path):
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer yes --fence-length 100000 > dat_tmp/figure_{}_{}_raw_w_{}_ratio_{}.dat'.format(w, a, mode, int(total * (1 - ratio)), int(total * ratio), cardinality, index_path, number_of_rows, v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio )
    return cmd

###################################### cmd for cardinality #########################################
def gen_cmd_naive_cardinality(w, a, v, total, ratio, index_path, c, n): 
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer no > dat_tmp/figure_{}_{}_raw_w_{}_ratio_{}_c_{}.dat'.format(w, a, mode,int(total * (1 - ratio)), int(total * ratio), c, index_path, n, v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio ,c)
    return cmd

def gen_cmd_others_cardinality(w, a, v, total, ratio, index_path, c, n):
    cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer yes --fence-length 100000 > dat_tmp/figure_{}_{}_raw_w_{}_ratio_{}_c_{}.dat'.format(w, a, mode, int(total * (1 - ratio)), int(total * ratio), c, index_path, n, v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio ,c)
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
    f = open('dat/figure_naive_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_naive(num, 'naive', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_naive_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#naive throughput, ratio
def gen_figure_naive_throughput_ratio():
    print ('Figure naive_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat/figure_naive_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_naive(16, 'naive', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_naive_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# naive throughput, cardinality
def gen_figure_naive_throughput_cardinality(): 
    print ('Figure naive_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat/figure_naive_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_naive_cardinality(core_for_cardinality, 'naive', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_naive_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#naive latency, core
def gen_figure_naive_latency_core(): # latency
    print ('Figure naive_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat/figure_naive_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_naive(num, 'naive', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_naive_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
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
    f = open('dat/figure_naive_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_naive(16, 'naive', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_naive_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
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
    f = open('dat/figure_naive_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_naive_cardinality(core_for_cardinality, 'naive', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_naive_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
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
    f = open('dat/figure_ucb_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ucb', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_ucb_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#ucb throughput, ratio
def gen_figure_ucb_throughput_ratio():
    print ('Figure ucb_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat/figure_ucb_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ucb', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_ucb_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# ucb throughput, cardinality
def gen_figure_ucb_throughput_cardinality(): 
    print ('Figure ucb_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat/figure_ucb_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'ucb', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_ucb_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#ucb latency, core
def gen_figure_ucb_latency_core(): # latency
    print ('Figure ucb_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat/figure_ucb_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ucb', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_ucb_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
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
    f = open('dat/figure_ucb_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ucb', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_ucb_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
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
    f = open('dat/figure_ucb_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'ucb', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_ucb_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
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
    f = open('dat/figure_ub_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ub', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_ub_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#ub throughput, ratio
def gen_figure_ub_throughput_ratio():
    print ('Figure ub_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat/figure_ub_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ub', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_ub_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# ub throughput, cardinality
def gen_figure_ub_throughput_cardinality(): 
    print ('Figure ub_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat/figure_ub_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'ub', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_ub_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#ub latency, core
def gen_figure_ub_latency_core(): # latency
    print ('Figure ub_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat/figure_ub_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ub', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_ub_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
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
    f = open('dat/figure_ub_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ub', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_ub_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
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
    f = open('dat/figure_ub_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'ub', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_ub_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

############################################## NBUB-lk ##########################################################
def gen_figure_nbub_lk_throughput_core(): 
    print ('Figure nbub-lk_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat/figure_nbub-lk_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'nbub-lk', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_nbub-lk_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#nbub-lk throughput, ratio
def gen_figure_nbub_lk_throughput_ratio():
    print ('Figure nbub-lk_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat/figure_nbub-lk_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'nbub-lk', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_nbub-lk_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# nbub-lk throughput, cardinality
def gen_figure_nbub_lk_throughput_cardinality(): 
    print ('Figure nbub-lk_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat/figure_nbub-lk_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'nbub-lk', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_nbub-lk_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#nbub-lk latency, core
def gen_figure_nbub_lk_latency_core(): # latency
    print ('Figure nbub-lk_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat/figure_nbub-lk_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'nbub-lk', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_nbub-lk_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

#nbub-lk latency, ratio
def gen_figure_nbub_lk_latency_ratio():
    print ('Figure nbub-lk_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat/figure_nbub-lk_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'nbub-lk', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_nbub-lk_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

# nbub-lk latency, cardinality
def gen_figure_nbub_lk_latency_cardinality(): 
    print ('Figure nbub-lk_latency_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat/figure_nbub-lk_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'nbub-lk', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_nbub-lk_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

############################################## NBUB_LF ##########################################################
def gen_figure_nbub_lf_throughput_core(): 
    print ('Figure nbub-lf_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat/figure_nbub-lf_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'nbub-lf', 'no', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_nbub-lf_throughput_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#nbub_lf throughput, ratio
def gen_figure_nbub_lf_throughput_ratio():
    print ('Figure nbub-lf_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat/figure_nbub-lf_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'nbub-lf', 'no', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_nbub-lf_throughput_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()    

# nbub-lf throughput, cardinality
def gen_figure_nbub_lf_throughput_cardinality(): 
    print ('Figure nbub-lf_throughput_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat/figure_nbub-lf_throughput_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'nbub-lf', 'no', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = throughput_analysis('dat_tmp/figure_nbub-lf_throughput_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        f.write('{} {} \n'.format(num, ret))
    f.close()

#nbub_lf latency, core
def gen_figure_nbub_lf_latency_core(): # latency
    print ('Figure nbub-lf_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16, 24, 32]
    f = open('dat/figure_nbub-lf_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'nbub-lf', 'yes', 1000, ratio_percentage, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_nbub-lf_latency_raw_w_{}_ratio_{}.dat'.format(num, ratio_percentage))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

#nbub_lf latency, ratio
def gen_figure_nbub_lf_latency_ratio():
    print ('Figure nbub-lf_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.02, 0.05, 0.1, 0.2]
    f = open('dat/figure_nbub-lf_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'nbub-lf', 'yes', 1000, num, index_path)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_nbub-lf_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()

# nbub-lf latency, cardinality
def gen_figure_nbub_lf_latency_cardinality(): 
    print ('Figure nbub-lf_latency_cardinality')
    print ('-' * 10)
    cardinality_number = [16, 32, 64, 128, 256]
    f = open('dat/figure_nbub-lf_latency_cardinality.dat','w')
    for num in cardinality_number:
        index_path = "100M_{}".format(num)
        cmd = gen_cmd_others_cardinality(core_for_cardinality, 'nbub-lf', 'yes', 1000, ratio_percentage, index_path, num, ONE_HUNDRED_MILLION)
        print(cmd)
        os.system(cmd)
        ret = latency_analysis('dat_tmp/figure_nbub-lf_latency_raw_w_{}_ratio_{}_c_{}.dat'.format(core_for_cardinality, ratio_percentage, num))
        print(ret)
        print('\n')
        for i in range(0,len(ret)):
            f.write('{} {} \n'.format(num, ret[i]))
    f.close()


def cdf():
    cmd = "./build/nicolas -w 16 -a naive -m mix --number-of-queries 970 -r 30 -c 32 -i 100M_32/ -n 100000000 -v yes --fence-pointer no > dat_tmp/figure_naive_latency_raw_w_32_ratio_0.03_c_32.dat"
    cmd2 = "./build/nicolas -w 16 -a nbub-lk -m mix --number-of-queries 970 -r 30 -c 32 -i 100M_32/ -n 100000000 -v yes --fence-pointer yes --fence-length 100000 > dat_tmp/figure_nbub-lk_latency_raw_w_32_ratio_0.03_c_32.dat"
    cmd3 = "./build/nicolas -w 16 -a nbub-lf -m mix --number-of-queries 970 -r 30 -c 32 -i 100M_32/ -n 100000000 -v yes --fence-pointer yes --fence-length 100000 > dat_tmp/figure_nbub-lf_latency_raw_w_32_ratio_0.03_c_32.dat"
    cmd4 = "./build/nicolas -w 16 -a ub -m mix --number-of-queries 970 -r 30 -c 32 -i 100M_32/ -n 100000000 -v yes --fence-pointer yes --fence-length 100000 > dat_tmp/figure_ub_latency_raw_w_32_ratio_0.03_c_32.dat"
    cmd5 = "./build/nicolas -w 16 -a ucb -m mix --number-of-queries 970 -r 30 -c 32 -i 100M_32/ -n 100000000 -v yes --fence-pointer yes --fence-length 100000 > dat_tmp/figure_ucb_latency_raw_w_32_ratio_0.03_c_32.dat"
    os.system(cmd)
    os.system(cmd2)
    os.system(cmd3)
    os.system(cmd4)
    os.system(cmd5)

def gen_data_and_index():
    # generate index for core and ratio
    help_gen_data_and_index(number_of_rows, cardinality, index_path)

    # generate 100M_X   X = [16, 32, 64, 128, 256]
    cardinality_array = [16, 32, 64, 128, 256]
    for num in cardinality_array:
        help_gen_data_and_index(ONE_HUNDRED_MILLION, num, "100M_{}".format(num))
    
    # generate 1B_X   X = [16, 32, 64, 128, 256]
    cardinality_array = [100]
    for num in cardinality_array:
        help_gen_data_and_index(ONE_BILLION, num, "1B_{}".format(num))    

    

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

    # #nbub-lk
    gen_figure_nbub_lk_throughput_core()
    gen_figure_nbub_lk_throughput_ratio()
    gen_figure_nbub_lk_throughput_cardinality()
    gen_figure_nbub_lk_latency_core()
    gen_figure_nbub_lk_latency_ratio()
    gen_figure_nbub_lk_latency_cardinality()

    # #nbub_lf
    gen_figure_nbub_lf_throughput_core()
    gen_figure_nbub_lf_throughput_ratio()
    gen_figure_nbub_lf_throughput_cardinality()
    gen_figure_nbub_lf_latency_core()
    gen_figure_nbub_lf_latency_ratio()
    gen_figure_nbub_lf_latency_cardinality()

    #cdf
    #cdf()

    # figure1
    # os.system('python3 figure1.py')

def main():
    itr = 0
    cmd = 'mv graphs graphs_{}'
    cmd2 = "mv dat graphs"
    cmd4 = "mv dat_tmp graphs"
    while itr < 3:
        os.chdir(ROOT_PATH)
        datdir = 'dat'
        if os.path.exists(datdir) and os.path.isdir(datdir):
            print ('Deleting existing directory ./dat')
            shutil.rmtree(datdir)
        os.mkdir(datdir)

        dat_tmp = 'dat_tmp'
        if os.path.exists(dat_tmp) and os.path.isdir(dat_tmp):
            print ('Deleting existing directory ./dat_tmp')
            shutil.rmtree(dat_tmp)
        os.mkdir(dat_tmp)

        graphs = 'graphs'
        if os.path.exists(graphs) and os.path.isdir(graphs):
            print ('Deleting existing directory ./graphs')
            shutil.rmtree(graphs)
        os.mkdir(graphs)

        build()
        gen_data_and_index()
        run()
        os.system(cmd4)
        os.system(cmd2)
        os.system(cmd.format(itr))
        itr += 1
    print('Done!\n')


if __name__ == '__main__':
    main()
