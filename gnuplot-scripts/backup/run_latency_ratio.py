
from multiprocessing.pool import ApplyResult
from numbers import Rational
import queue
from re import A, U
import shutil
import sys
import os


ROOT_PATH = os.getcwd()

ossystem = os.system

def build():
    cmd = 'sh build.sh'
    os.system(cmd)

def gen_cmd_naive(w, a, v, total, ratio):
    cmd = './build/nicolas -w {} -a {} -m mix --number-of-queries {} -r {} -c 100 -i 100M_100/ -n 100000000 -v {} --fence-pointer no > figure_{}_{}_raw_w_{}_ratio_{}.dat'.format(w, a, int(total * (1 - ratio)), int(total * ratio), v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio )
    return cmd

def gen_cmd_others(w, a, v, total, ratio):
    cmd = './build/nicolas -w {} -a {} -m mix --number-of-queries {} -r {} -c 100 -i 100M_100/ -n 100000000 -v {} --fence-pointer yes --fence-length 100000 > figure_{}_{}_raw_w_{}_ratio_{}.dat'.format(w, a, int(total * (1 - ratio)), int(total * ratio), v, a, 'latency' if (v == 'yes') else 'throughput',w, ratio )
    return cmd

# def gen_naive_latency_cmd(w, total, ratio, index):
#     cmd = './build/nicolas -w {} -a naive -m mix --number-of-queries {} -r {} -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer no > figure_{}_naive_latency_raw_w_{}_ratio_{}.dat'.format(w, int(total * (1 - ratio)), int(total * ratio), index, w, ratio )
#     return cmd

def throughput_analysis(filename, w):
    f = open(filename)
    res = 0.0
    for line in f:
        a = line.split()
        if len(a) != 3 or a[0] != 'Throughput':
            continue
        res += float(a[1])
    return res / w

def latency_analysis(filename):
    f = open(filename)
    Qvec = [] # vec for operations
    Uvec = []
    Ivec = []
    Dvec = []
    Allvec = []
    res = []
    for line in f:
        a = line.split()
        if (len(a) != 2):
            continue
        elif line.startswith('Q '):
            Qvec.append(float(a[-1]))
        elif line.startswith('U '):
            Uvec.append(float(a[-1]))
            Allvec.append(float(a[-1]))
        elif line.startswith('I '):
            Ivec.append(float(a[-1]))
            Allvec.append(float(a[-1]))
        elif line.startswith('D '):
            Dvec.append(float(a[-1]))
            Allvec.append(float(a[-1]))
        else:
            continue
    if len(Qvec) != 0:
        res.append(sum(Qvec) / len(Qvec)) 
    else:
        res.append(0)
    if len(Uvec) != 0:
        res.append(sum(Uvec) / len(Uvec)) 
    else:
        res.append(0)
    if len(Ivec) != 0:
        res.append(sum(Ivec) / len(Ivec)) 
    else:
        res.append(0)
    if len(Dvec) != 0:
        res.append(sum(Dvec) / len(Dvec)) 
    else:
        res.append(0)
    if len(Allvec) != 0:
        res.append(sum(Allvec) / len(Allvec)) 
    else:
        res.append(0)
    return res

    
############################################## NAIVE ############################################################
# naive throughput, core
def gen_figure_naive_throughput_core(): 
    print ('Figure naive_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_naive_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_naive(num, 'naive', 'no', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_naive_throughput_raw_w_{}_ratio_{}.dat'.format(num, 0.1), num)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()

#naive throughput, ratio
def gen_figure_naive_throughput_ratio():
    print ('Figure naive_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_naive_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_naive(16, 'naive', 'no', 1000, num)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_naive_throughput_raw_w_{}_ratio_{}.dat'.format(16, num), 16)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()    

#naive latency, core
def gen_figure_naive_latency_core(): # latency
    print ('Figure naive_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_naive_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_naive(num, 'naive', 'yes', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_naive_latency_raw_w_{}_ratio_{}.dat'.format(num, 0.1))
        print(res)
        print('\n')
        for i in range(0,len(core_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

#naive latency, ratio
def gen_figure_naive_latency_ratio():
    print ('Figure naive_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_naive_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_naive(16, 'naive', 'yes', 1000, num)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_naive_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(res)
        print('\n')
        for i in range(0,len(ratio_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

############################################## UCB ############################################################

# ucb throughput, core
def gen_figure_ucb_throughput_core(): 
    print ('Figure ucb_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_ucb_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ucb', 'no', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_ucb_throughput_raw_w_{}_ratio_{}.dat'.format(num, 0.1), num)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()

#ucb throughput, ratio
def gen_figure_ucb_throughput_ratio():
    print ('Figure ucb_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_ucb_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ucb', 'no', 1000, num)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_ucb_throughput_raw_w_{}_ratio_{}.dat'.format(16, num), 16)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()    

#ucb latency, core
def gen_figure_ucb_latency_core(): # latency
    print ('Figure ucb_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_ucb_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ucb', 'yes', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_ucb_latency_raw_w_{}_ratio_{}.dat'.format(num, 0.1))
        print(res)
        print('\n')
        for i in range(0,len(core_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

#ucb latency, ratio
def gen_figure_ucb_latency_ratio():
    print ('Figure ucb_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_ucb_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ucb', 'yes', 1000, num)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_ucb_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(res)
        print('\n')
        for i in range(0,len(ratio_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

############################################## UB ##########################################################
# ub throughput, core
def gen_figure_ub_throughput_core(): 
    print ('Figure ub_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_ub_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ub', 'no', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_ub_throughput_raw_w_{}_ratio_{}.dat'.format(num, 0.1), num)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()

#ub throughput, ratio
def gen_figure_ub_throughput_ratio():
    print ('Figure ub_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_ub_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ub', 'no', 1000, num)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_ub_throughput_raw_w_{}_ratio_{}.dat'.format(16, num), 16)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()    

#ub latency, core
def gen_figure_ub_latency_core(): # latency
    print ('Figure ub_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_ub_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'ub', 'yes', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_ub_latency_raw_w_{}_ratio_{}.dat'.format(num, 0.1))
        print(res)
        print('\n')
        for i in range(0,len(core_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

#ub latency, ratio
def gen_figure_ub_latency_ratio():
    print ('Figure ub_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_ub_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'ub', 'yes', 1000, num)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_ub_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(res)
        print('\n')
        for i in range(0,len(ratio_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

############################################## NBUB ##########################################################
def gen_figure_cubit_throughput_core(): 
    print ('Figure cubit_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_cubit_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'cubit', 'no', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_cubit_throughput_raw_w_{}_ratio_{}.dat'.format(num, 0.1), num)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()

#cubit throughput, ratio
def gen_figure_cubit_throughput_ratio():
    print ('Figure cubit_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_cubit_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'cubit', 'no', 1000, num)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_cubit_throughput_raw_w_{}_ratio_{}.dat'.format(16, num), 16)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()    

#cubit latency, core
def gen_figure_cubit_latency_core(): # latency
    print ('Figure cubit_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_cubit_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'cubit', 'yes', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_cubit_latency_raw_w_{}_ratio_{}.dat'.format(num, 0.1))
        print(res)
        print('\n')
        for i in range(0,len(core_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

#cubit latency, ratio
def gen_figure_cubit_latency_ratio():
    print ('Figure cubit_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_cubit_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'cubit', 'yes', 1000, num)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_cubit_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(res)
        print('\n')
        for i in range(0,len(ratio_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

############################################## NBUB_LF ##########################################################
def gen_figure_cubit_lf_throughput_core(): 
    print ('Figure cubit-lf_throughput_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_cubit-lf_throughput_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'cubit-lf', 'no', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_cubit-lf_throughput_raw_w_{}_ratio_{}.dat'.format(num, 0.1), num)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()

#cubit_lf throughput, ratio
def gen_figure_cubit_lf_throughput_ratio():
    print ('Figure cubit-lf_throughput_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_cubit-lf_throughput_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'cubit-lf', 'no', 1000, num)
        print(cmd)
        os.system(cmd)
        res = throughput_analysis('figure_cubit-lf_throughput_raw_w_{}_ratio_{}.dat'.format(16, num), 16)
        print(res)
        print('\n')
        f.write('{} {} \n'.format(num, res))
    f.close()    

#cubit_lf latency, core
def gen_figure_cubit_lf_latency_core(): # latency
    print ('Figure cubit-lf_latency_core')
    print ('-' * 10)
    core_number = [1, 2, 4, 8, 16]
    f = open('dat/figure_cubit-lf_latency_core.dat','w')
    for num in core_number:
        cmd = gen_cmd_others(num, 'cubit-lf', 'yes', 1000, 0.1)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_cubit-lf_latency_raw_w_{}_ratio_{}.dat'.format(num, 0.1))
        print(res)
        print('\n')
        for i in range(0,len(core_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

#cubit_lf latency, ratio
def gen_figure_cubit_lf_latency_ratio():
    print ('Figure cubit-lf_latency_ratio')
    print ('-' * 10)
    ratio_number = [0.01, 0.05, 0.1, 0.2, 0.4]
    f = open('dat/figure_cubit-lf_latency_ratio.dat','w')
    for num in ratio_number:
        cmd = gen_cmd_others(16, 'cubit-lf', 'yes', 1000, num)
        print(cmd)
        os.system(cmd)
        res = latency_analysis('figure_cubit-lf_latency_raw_w_{}_ratio_{}.dat'.format(16, num))
        print(res)
        print('\n')
        for i in range(0,len(ratio_number)):
            f.write('{} {} \n'.format(num, res[i]))
    f.close()

def cdf():
    cmd = "./build/nicolas -w 16 -a naive -m mix --number-of-queries 950 -r 50 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer no > figure_naive_latency_raw_w_32_ratio_0.05.dat"
    cmd2 = "./build/nicolas -w 16 -a cubit -m mix --number-of-queries 950 -r 50 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 100000 > figure_cubit_latency_raw_w_32_ratio_0.05.dat"
    cmd3 = "./build/nicolas -w 16 -a cubit-lf -m mix --number-of-queries 950 -r 50 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 100000 > figure_cubit-lf_latency_raw_w_32_ratio_0.05.dat"
    cmd4 = "./build/nicolas -w 16 -a ub -m mix --number-of-queries 950 -r 50 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 100000 > figure_ub_latency_raw_w_32_ratio_0.05.dat"
    cmd5 = "./build/nicolas -w 16 -a ucb -m mix --number-of-queries 950 -r 50 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 100000 > figure_ucb_latency_raw_w_32_ratio_0.05.dat"
    os.system(cmd)
    os.system(cmd2)
    os.system(cmd3)
    os.system(cmd4)
    os.system(cmd5)

def run():
    # Group zero
    #naive
    # gen_figure_naive_throughput_core()
    # gen_figure_naive_throughput_ratio()
    # gen_figure_naive_latency_core()
    # gen_figure_naive_latency_ratio()

    #ucb
    # gen_figure_ucb_throughput_core()
    # gen_figure_ucb_throughput_ratio()
    # gen_figure_ucb_latency_core()
    # gen_figure_ucb_latency_ratio()

    #ub
    # gen_figure_ub_throughput_core()
    # gen_figure_ub_throughput_ratio()
    # gen_figure_ub_latency_core()
    gen_figure_ub_latency_ratio()

    #cubit
    # gen_figure_cubit_throughput_core()
    # gen_figure_cubit_throughput_ratio()
    # gen_figure_cubit_latency_core()
    gen_figure_cubit_latency_ratio()

    #cubit_lf
    # gen_figure_cubit_lf_throughput_core()
    # gen_figure_cubit_lf_throughput_ratio()
    # gen_figure_cubit_lf_latency_core()
    gen_figure_cubit_lf_latency_ratio()

    #cdf
    #cdf()

def gen_graph():
    os.chdir("gnuplot-scripts")
    #os.system("./check_dat_files.sh")
    #os.system("./prepare_normalized.sh")
    # os.system("rm -r ../graphs")
    os.system("make make_dir")
    # os.system("make figure_multiple")
    os.system("make figure_test")
    os.chdir("../graphs")
    os.system('echo "Figures generated in \"`pwd`\""')
    #os.system('ls -l')

    # gen_cdf 
    # os.chdir("../")
    # os.system("python3 eva-scripts/cdf_core.py > graphs/cdf_output.txt")

def main():
    os.chdir(ROOT_PATH)
    datdir = 'dat'
    # if os.path.exists(datdir) and os.path.isdir(datdir):
    #     print ('Deleting existing directory ./dat')
    #     shutil.rmtree(datdir)
    if not os.path.exists(datdir):
        os.mkdir(datdir)
    # os.system("rm *.dat")

    #gen_data()
    #build()
    #build_index()
    run()
    gen_graph()
    print('Done!\n')


if __name__ == '__main__':
    main()