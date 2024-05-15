
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

index_path = "100M_100/" # dataset (-i)

cardinality = 100 # -c

number_of_rows = ONE_HUNDRED_MILLION # -n

mode = "mix" # -m

MERGE_THRESHOLD_NBUB = 20

# ROWS_PER_SEG = 100000  # 0.1M

NUM_EMBARR_PARALLEL = 2

PARALLEL_CNT = 'true'


######################################### global parameters ##########################################

ratio_percentage = 0.1

core_for_Seg = 16

seg_num = [1, 10, 100, 1000, 10000] 

def help_gen_data_and_index(N, C, index_path): # N for number_of_rows; C for cardinality
    cmd = 'python generate.py {} {}'
    os.system(cmd.format(N, C))
    cmd2 = './build/nicolas -m build -d {} -c {} -n {} -i {}'
    os.system(cmd2.format("a_{}_{}".format(N, C), C, N, index_path))


###################################### cmd for Seg #########################################
def gen_cmd_cubit_Seg(w, a, v, total, ratio, index_path, segs):
    if segs == 1:
        cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer yes --fence-length 100000 --rows-per-seg {} --num-embarr-parallel {} --parallel-cnt {} > dat_tmp_Seg/figure_{}_{}_raw_w_{}_ratio_{}_Segs_{}.dat'.format(w, a, mode, int(total * (1 - ratio)), int(total * ratio), cardinality, index_path, number_of_rows, v, int(number_of_rows * 0.95), NUM_EMBARR_PARALLEL, PARALLEL_CNT, a, 'latency' if (v == 'yes') else 'throughput',w, ratio, segs)
        return cmd
    else:
        cmd = './build/nicolas -w {} -a {} -m {} --number-of-queries {} -r {} -c {} -i {} -n {} -v {} --fence-pointer yes --fence-length 100000 --rows-per-seg {} --num-embarr-parallel {} --parallel-cnt {} > dat_tmp_Seg/figure_{}_{}_raw_w_{}_ratio_{}_Segs_{}.dat'.format(w, a, mode, int(total * (1 - ratio)), int(total * ratio), cardinality, index_path, number_of_rows, v, int(number_of_rows / segs), NUM_EMBARR_PARALLEL, PARALLEL_CNT, a, 'latency' if (v == 'yes') else 'throughput',w, ratio, segs)
        return cmd


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


############################################## NBUB-lk ##########################################################
#MT
# def gen_figure_cubit_lk_latency_MT():
#     print ('Figure cubit-lk_latency_MT')
#     print ('-' * 10)    
#     f = open('dat/figure_cubit-lk_latency_MT.dat','w')
#     for num in merge_threshold_num:
#         cmd = gen_cmd_others_MT(core_for_MT, 'cubit-lk', 'yes', 1000, ratio_percentage, index_path, num)
#         print(cmd)
#         os.system(cmd) 
#         ret = latency_analysis('dat_tmp_MT/figure_cubit-lk_latency_raw_w_{}_ratio_{}_MT_{}.dat'.format(core_for_MT, ratio_percentage, num))  
#         print(ret)     
#         print('\n')
#         # output for gnuplot
#     f.close()
############################################## NBUB_LF ##########################################################
#MT
def gen_figure_cubit_lf_latency_Seg():
    print ('Figure cubit-lf_latency_Seg')
    print ('-' * 10)    
    ret = [[]]
    ret.clear()
    for num in seg_num:
        temp_list = []
        temp_list.clear()
        cmd = gen_cmd_cubit_Seg(core_for_Seg, 'cubit-lk', 'yes', 1000, ratio_percentage, index_path, num)
        print(cmd)
        os.system(cmd) 
        res = latency_analysis('dat_tmp_Seg/figure_cubit-lk_latency_raw_w_{}_ratio_{}_Segs_{}.dat'.format(core_for_Seg, ratio_percentage, num))  
        print(res)     
        print('\n')
        temp_list.append(res[0])
        temp_list.append(res[4])
        ret.append(temp_list)

    return ret

def run():

    # #cubit_lf
    f = open('dat/figure_cubit-lk_latency_Seg.dat','w')
    ret = gen_figure_cubit_lf_latency_Seg()
    f.write('{} 1\n'.format(ret[0][0]))
    f.write('{} 2\n'.format(ret[0][1]))
    f.write('{} 4\n'.format(ret[1][0]))
    f.write('{} 5\n'.format(ret[1][1]))
    f.write('{} 7\n'.format(ret[2][0]))
    f.write('{} 8\n'.format(ret[2][1]))
    f.write('{} 10\n'.format(ret[3][0]))
    f.write('{} 11\n'.format(ret[3][1]))
    f.write('{} 13\n'.format(ret[4][0]))
    f.write('{} 14\n'.format(ret[4][1]))
    f.close()

def main():
    itr = 0
    cmd = 'mv graphs_Seg graphs_Seg_{}'
    cmd2 = "mv dat graphs_Seg"
    cmd4 = "mv dat_tmp_Seg graphs_Seg"
    while itr < 2:
        os.chdir(ROOT_PATH)

        # generate Seg
        if (os.path.isfile("100M_100")):
            print("100M_100 exists")
        else:
            help_gen_data_and_index(number_of_rows, cardinality, index_path)
        datdir = 'dat'
        if os.path.exists(datdir) and os.path.isdir(datdir):
            print ('Deleting existing directory ./dat')
            shutil.rmtree(datdir)
        os.mkdir(datdir)

        dat_tmp_Seg = 'dat_tmp_Seg'
        if os.path.exists(dat_tmp_Seg) and os.path.isdir(dat_tmp_Seg):
            print ('Deleting existing directory ./dat_tmp_Seg')
            shutil.rmtree(dat_tmp_Seg)
        os.mkdir(dat_tmp_Seg)

        graphs = 'graphs_Seg'
        if os.path.exists(graphs) and os.path.isdir(graphs):
            print ('Deleting existing directory ./graphs_Seg')
            shutil.rmtree(graphs)
        os.mkdir(graphs)

        run()
        os.system(cmd4)
        # os.system(cmd3)
        os.system(cmd2)       
        os.system(cmd.format(itr))
        itr += 1
    print('Done!\n')


if __name__ == '__main__':
    main()
