import sys
import os
import shutil
from analysis import *

#this needs to be adjusted!
#ROOT_PATH = '/path/to/the/root/directory/of/the/script'
#ROOT_PATH = '/home/junchang/hdd/NB-UpBit'
ROOT_PATH = os.getcwd()

ONE_HUNDRED_MILLION = str(100000000)
ONE_HUNDRED_MILLION_DATA = 'a_' + ONE_HUNDRED_MILLION + '_100'
ONE_BILLION = ONE_HUNDRED_MILLION + '0'
ONE_BILLION_DATA = 'a_' + ONE_BILLION + '_100'
ONE_MILLION = '1000000'

ossystem = os.system

def _system(msg):
    print msg
    ossystem(msg)
    # ossystem('perflock ' + msg)

os.system = _system

def gen_data():
    cmd = 'python generate.py {} {}'
    os.system(cmd.format(ONE_HUNDRED_MILLION, 100))
    os.system(cmd.format(ONE_BILLION, 100))
    os.system(cmd.format(ONE_HUNDRED_MILLION, 1000))
    os.system('python gene_zipf.py')

def build():
    cmd = 'sh build-light.sh'
    os.system(cmd)

def build_index():
    cmd = './build/nicolas -m build -d {} -c {} -n {} -i {}'
    os.system(cmd.format(ONE_HUNDRED_MILLION_DATA, 100, ONE_HUNDRED_MILLION, '100M_100'))
    os.system(cmd.format(ONE_BILLION_DATA, 100, ONE_BILLION, '1B_100'))
    os.system(cmd.format('a_' + ONE_HUNDRED_MILLION + '_1000', 1000, ONE_HUNDRED_MILLION, '100M_1000'))
    os.system(cmd.format('zipf_15', 100, ONE_HUNDRED_MILLION, '100M_zipf15'))

def gen_figure_1():
    print 'Figure 1'
    print '-' * 10
    cmd = './build/nicolas -a ucb -m update --number-of-queries 1000000 -r 1000000 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 10000000 > figure_1_ucb_raw.dat'
    os.system(cmd)
    cmd = 'python figure_1.py figure_1_ucb_raw.dat > dat/figure_1_ucb.dat'
    os.system(cmd)

def gen_figure_5():
    print 'Figure 5'
    print '-' * 10
    cmd = './build/nicolas -a naive -m mix --number-of-queries 90000 -r 10000 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer no > figure_5_naive_raw.dat'
    os.system(cmd)
    cmd = './build/nicolas -a ucb -m mix --number-of-queries 90000 -r 10000 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 10000000 > figure_5_ucb_raw.dat'
    os.system(cmd)
    to_naive = latency('figure_5_naive_raw.dat')
    tq_naive = latency_query('figure_5_naive_raw.dat')
    tu_naive = latency_update('figure_5_naive_raw.dat')
    to_ucb = latency('figure_5_ucb_raw.dat')
    tq_ucb = latency_query('figure_5_ucb_raw.dat')
    tu_ucb = latency_update('figure_5_ucb_raw.dat')
    f = open('dat/breakdown_throughput.dat', 'w')
    f.write('{} 1\n'.format(to_naive))
    f.write('{} 2\n'.format(to_ucb))
    f.write('{} 4\n'.format(tu_naive))
    f.write('{} 5\n'.format(tu_ucb))
    f.write('{} 7\n'.format(tq_naive))
    f.write('{} 8\n'.format(tq_ucb))
    f.close()

def gen_figure_6():
    # need to parse breakdown
    print 'Figure 6'
    print '-' * 10
    cmd = './build/nicolas -a ucb -m update --number-of-queries 410010 -r 410010 -c 2 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 100000000 > figure_6.dat'
    os.system(cmd)
    u_d = '''5 {} "Update" "UCB Mem 1st"
6 0 "Query" "  "
6 0 "Query" "  "
8 {} "Update" "UCB Mem 1K"
6 0 "Query" "  "
6 0 "Query" "  "
11 {} "Update" "UCB Mem 10K"
6 0 "Query" "  "
6 0 "Query" "  "
14 {} "Update" "UCB Mem 100K"
6 0 "Query" "  "
6 0 "Query" "  "
17 {} "Update" "UCB Mem 1M"'''.format(*get_ud('figure_6.dat'))
    f = open('dat/intro_U_D.dat', 'w')
    f.write(u_d)
    eb = '''5 {} "Update" "UCB Mem 1st"
6 0 "" "  "
3 0 "" " "
8 {} "Update" "UCB Mem 1K"
6 0 "" "  "
3 0 "" " "
11 {} "Update" "UCB Mem 10K"
6 0 "" "  "
3 0 "" " "
14 {} "Update" "UCB Mem 200K"
6 0 "" "  "
3 0 "" " "
17 {} "Update" "UCB Mem 1M"'''.format(*get_eb('figure_6.dat'))
    f = open('dat/intro_EB.dat', 'w')
    f.write(eb)
    q_d = '''3 0 "U" " "
5 {} "S" "UCB Mem 1st"
6 0 "" "  "
3 0 "U" " "
8 {} "S" "UCB Mem 1K"
6 0 "" "  "
3 0 "U" " "
11 {} "S" "UCB Mem 10K"
6 0 "" "  "
3 0 "U" " "
14 {} "S" "UCB Mem 200K"
6 0 "" "  "
3 0 "U" " "
17 {} "S" "UCB Mem 1M"'''.format(*get_qd('figure_6.dat'))
    f = open('dat/intro_Q_D.dat', 'w')
    f.write(q_d)
    q_a = '''3 0 "U" " "
5 {} "R" "UCB Mem 1st"
3 0 "" " "
3 0 "U" " "
8 {} "R" "UCB Mem 1K"
6 0 "" "  "
3 0 "U" " "
11 {} "R" "UCB Mem 10K"
6 0 "" "  "
3 0 "U" " "
14 {} "R" "UCB Mem 200K"
3 0 "" " "
6 0 "U" "  "
17 {} "R" "UCB Mem 1M"'''.format(*get_qa('figure_6.dat'))
    f = open('dat/intro_Q_A.dat', 'w')
    f.write(q_a)

def gen_figure_9():
    print 'Figure 9'
    print '-' * 10
    cmd = './build/nicolas -a ub -m update --number-of-queries 1000000 -r 1000000 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 10000 > figure_1_ub_raw.dat'
    os.system(cmd)
    cmd = 'python figure_1.py figure_1_ub_raw.dat > dat/figure_1_ub.dat'
    os.system(cmd)

def gen_figure_10_meta(m, d, num):
    time_limit = 9999
    # 1%
    cmd = './build/nicolas -a naive -m {} --number-of-queries 9900 -r 100 -c 100 -i {} -n 100000000 -v yes --fence-pointer no -t {} > figure_{}_naive_1_raw.dat'.format(m, d, time_limit, num)
    os.system(cmd)
    cmd = './build/nicolas -a ucb -m {} --number-of-queries 9900 -r 100 -c 100 -i {} -n 100000000 -v yes --fence-pointer yes --fence-length 10000000 -t {} > figure_{}_ucb_1_raw.dat'.format(m, d, time_limit, num)
    os.system(cmd)
    cmd = './build/nicolas -a ub -m {} --number-of-queries 9900 -r 100 -c 100 -i {} -n 100000000 -v yes --fence-pointer yes --fence-length 10000 -t {} > figure_{}_ub_1_raw.dat'.format(m, d, time_limit, num)
    os.system(cmd)
    # 5%
    cmd = './build/nicolas -a naive -m {} --number-of-queries 9500 -r 500 -c 100 -i {} -n 100000000 -v yes --fence-pointer no -t {} > figure_{}_naive_5_raw.dat'.format(m, d, time_limit, num)
    os.system(cmd)
    cmd = './build/nicolas -a ucb -m {} --number-of-queries 9500 -r 500 -c 100 -i {} -n 100000000 -v yes --fence-pointer yes --fence-length 10000000 -t {} > figure_{}_ucb_5_raw.dat'.format(m, d, time_limit, num)
    os.system(cmd)
    cmd = './build/nicolas -a ub -m {} --number-of-queries 9500 -r 500 -c 100 -i {} -n 100000000 -v yes --fence-pointer yes --fence-length 10000 -t {} > figure_{}_ub_5_raw.dat'.format(m, d, time_limit, num)
    os.system(cmd)
    # 10%
    cmd = './build/nicolas -a naive -m {} --number-of-queries 9000 -r 1000 -c 100 -i {} -n 100000000 -v yes --fence-pointer no -t {} > figure_{}_naive_10_raw.dat'.format(m, d, time_limit, num)
    os.system(cmd)
    cmd = './build/nicolas -a ucb -m {} --number-of-queries 9000 -r 1000 -c 100 -i {} -n 100000000 -v yes --fence-pointer yes --fence-length 10000000 -t {} > figure_{}_ucb_10_raw.dat'.format(m, d, time_limit, num)
    os.system(cmd)
    cmd = './build/nicolas -a ub -m {} --number-of-queries 9000 -r 1000 -c 100 -i {} -n 100000000 -v yes --fence-pointer yes --fence-length 10000 -t {} > figure_{}_ub_10_raw.dat'.format(m, d, time_limit, num)
    os.system(cmd)

def gen_figure_10():
    print 'Figure 10'
    print '-' * 10
    gen_figure_10_meta('update', '100M_100/', 10)
    f = open('dat/update_breakup.dat', 'w')
    f = open('dat/update_breakup.dat', 'w')
    f.write('1  {} {} "In-place"\n'.format(latency_update('figure_10_naive_1_raw.dat'), latency_query('figure_10_naive_1_raw.dat')))
    f.write('2  {} {} "UCB"\n'.format(latency_update('figure_10_ucb_1_raw.dat'), latency_query('figure_10_ucb_1_raw.dat')))
    f.write('3  {} {} "UpBit"\n'.format(latency_update('figure_10_ub_1_raw.dat'), latency_query('figure_10_ub_1_raw.dat')))
    f.write('5  {} {} "In-place"\n'.format(latency_update('figure_10_naive_5_raw.dat'), latency_query('figure_10_naive_5_raw.dat')))
    f.write('6  {} {} "UCB"\n'.format(latency_update('figure_10_ucb_5_raw.dat'), latency_query('figure_10_ucb_5_raw.dat')))
    f.write('7  {} {} "UpBit"\n'.format(latency_update('figure_10_ub_5_raw.dat'), latency_query('figure_10_ub_5_raw.dat')))
    f.write('9  {} {} "In-place"\n'.format(latency_update('figure_10_naive_10_raw.dat'), latency_query('figure_10_naive_10_raw.dat')))
    f.write('10 {} {} "UCB"\n'.format(latency_update('figure_10_ucb_10_raw.dat'), latency_query('figure_10_ucb_10_raw.dat')))
    f.write('11 {} {} "UpBit"\n'.format(latency_update('figure_10_ub_10_raw.dat'), latency_query('figure_10_ub_10_raw.dat')))
    f.close()
    # reuse dat for Figure12a
    f = open('dat/update_aggregate.dat', 'w')
    f.write('1  {1} {0} "In-place"\n'.format(workload_latency_update('figure_10_naive_1_raw.dat'), workload_latency_query('figure_10_naive_1_raw.dat')))
    f.write('2  {1} {0} "UCB"\n'.format(workload_latency_update('figure_10_ucb_1_raw.dat'), workload_latency_query('figure_10_ucb_1_raw.dat')))
    f.write('3  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_10_ub_1_raw.dat'), workload_latency_query('figure_10_ub_1_raw.dat')))
    f.write('4  0         0 ""\n')
    f.write('5  {1} {0} "In-place"\n'.format(workload_latency_update('figure_10_naive_5_raw.dat'), workload_latency_query('figure_10_naive_5_raw.dat')))
    f.write('6  {1} {0} "UCB"\n'.format(workload_latency_update('figure_10_ucb_5_raw.dat'), workload_latency_query('figure_10_ucb_5_raw.dat')))
    f.write('7  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_10_ub_5_raw.dat'), workload_latency_query('figure_10_ub_5_raw.dat')))
    f.write('8  0         0 ""\n')
    f.write('9  {1} {0} "In-place"\n'.format(workload_latency_update('figure_10_naive_10_raw.dat'), workload_latency_query('figure_10_naive_10_raw.dat')))
    f.write('10 {1} {0} "UCB"\n'.format(workload_latency_update('figure_10_ucb_10_raw.dat'), workload_latency_query('figure_10_ucb_10_raw.dat')))
    f.write('11 {1} {0} "UpBit"\n'.format(workload_latency_update('figure_10_ub_10_raw.dat'), workload_latency_query('figure_10_ub_10_raw.dat')))
    f.close()

def gen_figure_11():
    '''Same as Figure 10'''
    print 'Figure 11'
    print '-' * 10

def gen_figure_12():
    print 'Figure 12'
    print '-' * 10
    gen_figure_10_meta('delete', '100M_100/', 11)
    f = open('dat/delete_new.dat', 'w')
    f.write('1  {1} {0} "In-place"\n'.format(workload_latency_update('figure_11_naive_1_raw.dat'), workload_latency_query('figure_11_naive_1_raw.dat')))
    f.write('2  {1} {0} "UCB"\n'.format(workload_latency_update('figure_11_ucb_1_raw.dat'), workload_latency_query('figure_11_ucb_1_raw.dat')))
    f.write('3  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_11_ub_1_raw.dat'), workload_latency_query('figure_11_ub_1_raw.dat')))
    f.write('5  {1} {0} "In-place"\n'.format(workload_latency_update('figure_11_naive_5_raw.dat'), workload_latency_query('figure_11_naive_5_raw.dat')))
    f.write('6  {1} {0} "UCB"\n'.format(workload_latency_update('figure_11_ucb_5_raw.dat'), workload_latency_query('figure_11_ucb_5_raw.dat')))
    f.write('7  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_11_ub_5_raw.dat'), workload_latency_query('figure_11_ub_5_raw.dat')))
    f.write('9  {1} {0} "In-place"\n'.format(workload_latency_update('figure_11_naive_10_raw.dat'), workload_latency_query('figure_11_naive_10_raw.dat')))
    f.write('10 {1} {0} "UCB"\n'.format(workload_latency_update('figure_11_ucb_10_raw.dat'), workload_latency_query('figure_11_ucb_10_raw.dat')))
    f.write('11 {1} {0} "UpBit"\n'.format(workload_latency_update('figure_11_ub_10_raw.dat'), workload_latency_query('figure_11_ub_10_raw.dat')))
    f.close()
    gen_figure_10_meta('insert', '100M_100/', 12)
    f = open('dat/insert_new.dat', 'w')
    f.write('1  {1} {0} "In-place"\n'.format(workload_latency_update('figure_12_naive_1_raw.dat'), workload_latency_query('figure_12_naive_1_raw.dat')))
    f.write('2  {1} {0} "UCB"\n'.format(workload_latency_update('figure_12_ucb_1_raw.dat'), workload_latency_query('figure_12_ucb_1_raw.dat')))
    f.write('3  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_12_ub_1_raw.dat'), workload_latency_query('figure_12_ub_1_raw.dat')))
    f.write('5  {1} {0} "In-place"\n'.format(workload_latency_update('figure_12_naive_5_raw.dat'), workload_latency_query('figure_12_naive_5_raw.dat')))
    f.write('6  {1} {0} "UCB"\n'.format(workload_latency_update('figure_12_ucb_5_raw.dat'), workload_latency_query('figure_12_ucb_5_raw.dat')))
    f.write('7  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_12_ub_5_raw.dat'), workload_latency_query('figure_12_ub_5_raw.dat')))
    f.write('9  {1} {0} "In-place"\n'.format(workload_latency_update('figure_12_naive_10_raw.dat'), workload_latency_query('figure_12_naive_10_raw.dat')))
    f.write('10 {1} {0} "UCB"\n'.format(workload_latency_update('figure_12_ucb_10_raw.dat'), workload_latency_query('figure_12_ucb_10_raw.dat')))
    f.write('11 {1} {0} "UpBit"\n'.format(workload_latency_update('figure_12_ub_10_raw.dat'), workload_latency_query('figure_12_ub_10_raw.dat')))
    f.close()

def gen_figure_13():
    print 'Figure 13'
    print '-' * 10
    gen_figure_10_meta('mix', '100M_100/', 13)
    f = open('dat/mix_new.dat', 'w')
    f.write('1  {1} {0} "In-place"\n'.format(workload_latency_update('figure_13_naive_1_raw.dat'), workload_latency_query('figure_13_naive_1_raw.dat')))
    f.write('2  {1} {0} "UCB"\n'.format(workload_latency_update('figure_13_ucb_1_raw.dat'), workload_latency_query('figure_13_ucb_1_raw.dat')))
    f.write('3  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_13_ub_1_raw.dat'), workload_latency_query('figure_13_ub_1_raw.dat')))
    f.write('4  0         0 ""\n')
    f.write('5  {1} {0} "In-place"\n'.format(workload_latency_update('figure_13_naive_5_raw.dat'), workload_latency_query('figure_13_naive_5_raw.dat')))
    f.write('6  {1} {0} "UCB"\n'.format(workload_latency_update('figure_13_ucb_5_raw.dat'), workload_latency_query('figure_13_ucb_5_raw.dat')))
    f.write('7  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_13_ub_5_raw.dat'), workload_latency_query('figure_13_ub_5_raw.dat')))
    f.write('8  0         0 ""\n')
    f.write('9  {1} {0} "In-place"\n'.format(workload_latency_update('figure_13_naive_10_raw.dat'), workload_latency_query('figure_13_naive_10_raw.dat')))
    f.write('10 {1} {0} "UCB"\n'.format(workload_latency_update('figure_13_ucb_10_raw.dat'), workload_latency_query('figure_13_ucb_10_raw.dat')))
    f.write('11 {1} {0} "UpBit"\n'.format(workload_latency_update('figure_13_ub_10_raw.dat'), workload_latency_query('figure_13_ub_10_raw.dat')))
    f.close()

def gen_figure_14():
    print 'Figure 14'
    print '-' * 10
    gen_figure_10_meta('mix', '100M_1000', 14)
    f = open('dat/mix_1000.dat', 'w')
    f.write('1  {1} {0} "In-place"\n'.format(workload_latency_update('figure_14_naive_1_raw.dat'), workload_latency_query('figure_14_naive_1_raw.dat')))
    f.write('2  {1} {0} "UCB"\n'.format(workload_latency_update('figure_14_ucb_1_raw.dat'), workload_latency_query('figure_14_ucb_1_raw.dat')))
    f.write('3  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_14_ub_1_raw.dat'), workload_latency_query('figure_14_ub_1_raw.dat')))
    f.write('4  0         0 ""\n')
    f.write('5  {1} {0} "In-place"\n'.format(workload_latency_update('figure_14_naive_5_raw.dat'), workload_latency_query('figure_14_naive_5_raw.dat')))
    f.write('6  {1} {0} "UCB"\n'.format(workload_latency_update('figure_14_ucb_5_raw.dat'), workload_latency_query('figure_14_ucb_5_raw.dat')))
    f.write('7  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_14_ub_5_raw.dat'), workload_latency_query('figure_14_ub_5_raw.dat')))
    f.write('8  0         0 ""\n')
    f.write('9  {1} {0} "In-place"\n'.format(workload_latency_update('figure_14_naive_10_raw.dat'), workload_latency_query('figure_14_naive_10_raw.dat')))
    f.write('10 {1} {0} "UCB"\n'.format(workload_latency_update('figure_14_ucb_10_raw.dat'), workload_latency_query('figure_14_ucb_10_raw.dat')))
    f.write('11 {1} {0} "UpBit"\n'.format(workload_latency_update('figure_14_ub_10_raw.dat'), workload_latency_query('figure_14_ub_10_raw.dat')))
    f.close()

def gen_figure_15():
    print 'Figure 15'
    print '-' * 10
    gen_figure_10_meta('mix', '1B_100', 15)
    f = open('dat/mix_1B.dat', 'w')
    f.write('1  {1} {0} "In-place"\n'.format(workload_latency_update('figure_15_naive_1_raw.dat'), workload_latency_query('figure_15_naive_1_raw.dat')))
    f.write('2  {1} {0} "UCB"\n'.format(workload_latency_update('figure_15_ucb_1_raw.dat'), workload_latency_query('figure_15_ucb_1_raw.dat')))
    f.write('3  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_15_ub_1_raw.dat'), workload_latency_query('figure_15_ub_1_raw.dat')))
    f.write('4  0         0 ""\n')
    f.write('5  {1} {0} "In-place"\n'.format(workload_latency_update('figure_15_naive_5_raw.dat'), workload_latency_query('figure_15_naive_5_raw.dat')))
    f.write('6  {1} {0} "UCB"\n'.format(workload_latency_update('figure_15_ucb_5_raw.dat'), workload_latency_query('figure_15_ucb_5_raw.dat')))
    f.write('7  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_15_ub_5_raw.dat'), workload_latency_query('figure_15_ub_5_raw.dat')))
    f.write('8  0         0 ""\n')
    f.write('9  {1} {0} "In-place"\n'.format(workload_latency_update('figure_15_naive_10_raw.dat'), workload_latency_query('figure_15_naive_10_raw.dat')))
    f.write('10 {1} {0} "UCB"\n'.format(workload_latency_update('figure_15_ucb_10_raw.dat'), workload_latency_query('figure_15_ucb_10_raw.dat')))
    f.write('11 {1} {0} "UpBit"\n'.format(workload_latency_update('figure_15_ub_10_raw.dat'), workload_latency_query('figure_15_ub_10_raw.dat')))
    f.close()

def gen_figure_16():
    print 'Figure 16'
    print '-' * 10
    gen_figure_10_meta('mix', '100M_zipf15', 16)
    f = open('dat/mix_zipf.dat', 'w')
    f.write('1  {1} {0} "In-place"\n'.format(workload_latency_update('figure_16_naive_1_raw.dat'), workload_latency_query('figure_16_naive_1_raw.dat')))
    f.write('2  {1} {0} "UCB"\n'.format(workload_latency_update('figure_16_ucb_1_raw.dat'), workload_latency_query('figure_16_ucb_1_raw.dat')))
    f.write('3  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_16_ub_1_raw.dat'), workload_latency_query('figure_16_ub_1_raw.dat')))
    f.write('4  0         0 ""\n')
    f.write('5  {1} {0} "In-place"\n'.format(workload_latency_update('figure_16_naive_5_raw.dat'), workload_latency_query('figure_16_naive_5_raw.dat')))
    f.write('6  {1} {0} "UCB"\n'.format(workload_latency_update('figure_16_ucb_5_raw.dat'), workload_latency_query('figure_16_ucb_5_raw.dat')))
    f.write('7  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_16_ub_5_raw.dat'), workload_latency_query('figure_16_ub_5_raw.dat')))
    f.write('8  0         0 ""\n')
    f.write('9  {1} {0} "In-place"\n'.format(workload_latency_update('figure_16_naive_10_raw.dat'), workload_latency_query('figure_16_naive_10_raw.dat')))
    f.write('10 {1} {0} "UCB"\n'.format(workload_latency_update('figure_16_ucb_10_raw.dat'), workload_latency_query('figure_16_ucb_10_raw.dat')))
    f.write('11 {1} {0} "UpBit"\n'.format(workload_latency_update('figure_16_ub_10_raw.dat'), workload_latency_query('figure_16_ub_10_raw.dat')))
    f.close()

def gen_figure_17():
    print 'Figure 17'
    print '-' * 10
    # Berkeley Earth
    os.system('python gen_earth.py')
    os.system('./build/nicolas -m build -d a_earth -c 114 -n 31648320 -i earth_new')
    m = 'mix'
    d = 'earth_new'
    # 1%
    cmd = './build/nicolas -a naive -m {} --number-of-queries 99000 -r 1000 -c 114 -i {} -n 31648320 -v yes --fence-pointer no > figure_17_naive_1_raw.dat'.format(m, d)
    os.system(cmd)
    cmd = './build/nicolas -a ucb -m {} --number-of-queries 99000 -r 1000 -c 114 -i {} -n 31648320 -v yes --fence-pointer yes --fence-length 10000000 > figure_17_ucb_1_raw.dat'.format(m, d)
    os.system(cmd)
    cmd = './build/nicolas -a ub -m {} --number-of-queries 99000 -r 1000 -c 114 -i {} -n 31648320 -v yes --fence-pointer yes --fence-length 10000 > figure_17_ub_1_raw.dat'.format(m, d)
    os.system(cmd)
    # 5%
    cmd = './build/nicolas -a naive -m {} --number-of-queries 95000 -r 5000 -c 114 -i {} -n 31648320 -v yes --fence-pointer no > figure_17_naive_5_raw.dat'.format(m, d)
    os.system(cmd)
    cmd = './build/nicolas -a ucb -m {} --number-of-queries 95000 -r 5000 -c 114 -i {} -n 31648320 -v yes --fence-pointer yes --fence-length 10000000 > figure_17_ucb_5_raw.dat'.format(m, d)
    os.system(cmd)
    cmd = './build/nicolas -a ub -m {} --number-of-queries 95000 -r 5000 -c 114 -i {} -n 31648320 -v yes --fence-pointer yes --fence-length 10000 > figure_17_ub_5_raw.dat'.format(m, d)
    os.system(cmd)
    # 10%
    cmd = './build/nicolas -a naive -m {} --number-of-queries 90000 -r 10000 -c 114 -i {} -n 31648320 -v yes --fence-pointer no > figure_17_naive_10_raw.dat'.format(m, d)
    os.system(cmd)
    cmd = './build/nicolas -a ucb -m {} --number-of-queries 90000 -r 10000 -c 114 -i {} -n 31648320 -v yes --fence-pointer yes --fence-length 10000000 > figure_17_ucb_10_raw.dat'.format(m, d)
    os.system(cmd)
    cmd = './build/nicolas -a ub -m {} --number-of-queries 90000 -r 10000 -c 114 -i {} -n 31648320 -v yes --fence-pointer yes --fence-length 10000 > figure_17_ub_10_raw.dat'.format(m, d)
    os.system(cmd)
    f = open('dat/mix_earth_10.dat', 'w')
    f.write('1  {1} {0} "In-place"\n'.format(workload_latency_update('figure_17_naive_1_raw.dat'), workload_latency_query('figure_17_naive_1_raw.dat')))
    f.write('2  {1} {0} "UCB"\n'.format(workload_latency_update('figure_17_ucb_1_raw.dat'), workload_latency_query('figure_17_ucb_1_raw.dat')))
    f.write('3  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_17_ub_1_raw.dat'), workload_latency_query('figure_17_ub_1_raw.dat')))
    f.write('4  0         0 ""\n')
    f.write('5  {1} {0} "In-place"\n'.format(workload_latency_update('figure_17_naive_5_raw.dat'), workload_latency_query('figure_17_naive_5_raw.dat')))
    f.write('6  {1} {0} "UCB"\n'.format(workload_latency_update('figure_17_ucb_5_raw.dat'), workload_latency_query('figure_17_ucb_5_raw.dat')))
    f.write('7  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_17_ub_5_raw.dat'), workload_latency_query('figure_17_ub_5_raw.dat')))
    f.write('8  0         0 ""\n')
    f.write('9  {1} {0} "In-place"\n'.format(workload_latency_update('figure_17_naive_10_raw.dat'), workload_latency_query('figure_17_naive_10_raw.dat')))
    f.write('10 {1} {0} "UCB"\n'.format(workload_latency_update('figure_17_ucb_10_raw.dat'), workload_latency_query('figure_17_ucb_10_raw.dat')))
    f.write('11 {1} {0} "UpBit"\n'.format(workload_latency_update('figure_17_ub_10_raw.dat'), workload_latency_query('figure_17_ub_10_raw.dat')))
    f.close()

def gen_figure_18():
    print 'Figure 18'
    print '-' * 10
    # VS Scanop
    def _gen_data_and_build_index(n, c):
        os.system('python generate.py {} {}'.format(n, c))
        os.system('./build/nicolas -m build -d {} -c {} -n {} -i {}'.format('a_{}_{}'.format(n, c), c, n, "{}_{}/".format(n, c)))
        pass
    cmd = './build/nicolas -a ub -m update --number-of-queries 20 -r 0 -c {c} -i {filename} -n {n} -v yes --fence-pointer yes --fence-length 100000 > figure_18_{n}_{c}.dat'
    n_c = [(10 ** 9, 1000), (10 ** 9, 100), (10 ** 9, 20), (10 ** 9, 10), (10 ** 9, 5), (10 ** 9, 2)]
    for n, c in n_c:
       _gen_data_and_build_index(n, c)
       os.system(cmd.format(c=c, n=n, filename='{}_{}/'.format(n, c)))
    _gen_data_and_build_index(10 ** 9, 1000)
    cmd = './build/nicolas -a ub -m range --number-of-queries 10 -r 0 -c {c} -i {filename} -n {n} -v yes --fence-pointer yes --fence-length 100000 --range {r} --range-algorithm pq > figure_18_{n}_{c}_{r}.dat'
    for r in [1, 10, 50, 100, 250, 500]:
       os.system(cmd.format(c=1000, n=10 ** 9, filename='1B_1000/', r=r))
       pass
    f = open('dat/upbit_vs_scan.dat', 'w')
    f.write('0.1 {} {} 0.\n'.format(latency_query('figure_18_1000000000_1000_1.dat') / 1000.0, latency_query('figure_18_{n}_{c}.dat'.format(n=n_c[0][0], c=n_c[0][1])) / 1000.0))
    f.write('1 {} {} 0.\n'.format(latency_query('figure_18_1000000000_1000_10.dat') / 1000.0, latency_query('figure_18_{n}_{c}.dat'.format(n=n_c[1][0], c=n_c[1][1])) / 1000.0))
    f.write('5 {} {} 0.\n'.format(latency_query('figure_18_1000000000_1000_50.dat') / 1000.0, latency_query('figure_18_{n}_{c}.dat'.format(n=n_c[2][0], c=n_c[2][1])) / 1000.0))
    f.write('10 {} {} 0.\n'.format(latency_query('figure_18_1000000000_1000_100.dat') / 1000.0, latency_query('figure_18_{n}_{c}.dat'.format(n=n_c[3][0], c=n_c[3][1])) / 1000.0))
    f.write('25 {} {} 0.\n'.format(latency_query('figure_18_1000000000_1000_250.dat') / 1000.0, latency_query('figure_18_{n}_{c}.dat'.format(n=n_c[4][0], c=n_c[4][1])) / 1000.0))
    f.write('50 {} {} 1.\n'.format(latency_query('figure_18_1000000000_1000_500.dat') / 1000.0, latency_query('figure_18_{n}_{c}.dat'.format(n=n_c[5][0], c=n_c[5][1])) / 1000.0))
    f.close()

def gen_figure_19_meta(f):
    f.write('{} 1\n'.format(latency_update('ub_merge_1')))
    f.write('{} 2\n'.format(latency_query('ub_merge_1')))
    f.write('{} 4\n'.format(latency_update('ub_merge_5')))
    f.write('{} 5\n'.format(latency_query('ub_merge_5')))
    f.write('{} 7\n'.format(latency_update('ub_merge_10')))
    f.write('{} 8\n'.format(latency_query('ub_merge_10')))
    f.write('{} 10\n'.format(latency_update('ub_merge_20')))
    f.write('{} 11\n'.format(latency_query('ub_merge_20')))
    f.write('{} 13\n'.format(latency_update('ub_merge_50')))
    f.write('{} 14\n'.format(latency_query('ub_merge_50')))
    f.write('{} 16\n'.format(latency_update('ub_merge_100')))
    f.write('{} 17\n'.format(latency_query('ub_merge_100')))
    f.write('{} 19\n'.format(latency_update('ub_merge_200')))
    f.write('{} 20\n'.format(latency_query('ub_merge_200')))
    f.write('{} 22\n'.format(latency_update('ub_merge_500')))
    f.write('{} 23\n'.format(latency_query('ub_merge_500')))
    f.write('{} 25\n'.format(latency_update('ub_merge_1000')))
    f.write('{} 26\n'.format(latency_query('ub_merge_1000')))

def gen_figure_19():
    print 'Figure 19'
    print '-' * 10
    os.system('sh merge.sh')
    f = open('dat/thres.dat', 'w')
    gen_figure_19_meta(f)
    f.close()
    os.system('sh merge_2.sh')
    f = open('dat/thres_0.2.dat', 'w')
    gen_figure_19_meta(f)
    f.close()

def gen_figure_20():
    print 'Figure 20'
    print '-' * 10
    os.system('sh fp.sh')
    f = open('dat/fp_ub.dat', 'w')
    f.write('{} "length = 10^0"\n'.format(latency('ub_fp_0')))
    f.write('1196723376\n')
    f.write('{} "length = 10^1"\n'.format(latency('ub_fp_10')))
    f.write('198467776\n')
    f.write('{} "length = 10^1"\n'.format(latency('ub_fp_100')))
    f.write('25054160\n')
    f.write('{} "length = 10^2"\n'.format(latency('ub_fp_1000')))
    f.write('2573480\n')
    f.write('{} "length = 10^3"\n'.format(latency('ub_fp_10000')))
    f.write('258400\n')
    f.write('{} "length = 10^4"\n'.format(latency('ub_fp_100000')))
    f.write('26400\n')
    f.write('{} "length = 10^5"\n'.format(latency('ub_fp_1000000')))
    f.write('2512\n')
    f.write('{} "length = 10^6"\n'.format(latency('ub_fp_10000000')))
    f.write('112\n')
    f.write('{} "no FP"\n'.format(latency('ub_fp_no')))
    f.write('0\n')

def gen_figure_21():
    print 'Figure 21'
    print '-' * 10
    v = {1: 1e8, 2: 1e8, 4: 1e8, 8: 1e8, 16: 1e8}
    for i in range(10):
        os.system('sh get_value.sh')
        for j in [1, 2, 4, 8, 16]:
            ff = open('getvalue_' + str(j))
            v[j] = min(v[j], float(ff.read()))
    f = open('dat/getvalue.dat', 'w')
    f.write('1 {}\n'.format(v[1]))
    f.write('2 {}\n'.format(v[2]))
    f.write('4 {}\n'.format(v[4]))
    f.write('8 {}\n'.format(v[8]))
    f.write('16 {}\n'.format(v[16]))
    f.close()


def gen_figure_23():
    print 'Figure 23'
    print '-' * 10
    f = open('dat/fp_ucb.dat', 'w')
    f.write('{} "length = 10^0"\n'.format(latency('ucb_fp_0')))
    f.write('1196723376\n')
    f.write('{} "length = 10^1"\n'.format(latency('ucb_fp_10')))
    f.write('198467776\n')
    f.write('{} "length = 10^2"\n'.format(latency('ucb_fp_100')))
    f.write('25054160\n')
    f.write('{} "length = 10^3"\n'.format(latency('ucb_fp_1000')))
    f.write('2573480\n')
    f.write('{} "no FP"\n'.format(latency('ucb_fp_no')))
    f.write('0\n')


def gen_figure_24():
    print 'Figure 24'
    print '-' * 10
    gen_figure_10_meta('update', '100M_100/', 24)
    cmd = './build/nicolas -a ucb -m update --number-of-queries 90000 -r 10000 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 10000 > figure_24_ucb_1_fp_raw.dat'
    os.system(cmd)
    cmd = './build/nicolas -a ucb -m update --number-of-queries 90000 -r 10000 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 10000 > figure_24_ucb_5_fp_raw.dat'
    os.system(cmd)
    cmd = './build/nicolas -a ucb -m update --number-of-queries 90000 -r 10000 -c 100 -i 100M_100/ -n 100000000 -v yes --fence-pointer yes --fence-length 10000 > figure_24_ucb_10_fp_raw.dat'
    os.system(cmd)
    f = open('dat/update_aggregate_fp.dat', 'w')
    f.write('1  {1} {0} "In-place"\n'.format(workload_latency_update('figure_24_naive_1_raw.dat'), workload_latency_query('figure_24_naive_1_raw.dat')))
    f.write('2  {1} {0} "UCB"\n'.format(workload_latency_update('figure_24_ucb_1_raw.dat'), workload_latency_query('figure_24_ucb_1_raw.dat')))
    f.write('3  {1} {0} "UpBit-FP"\n'.format(workload_latency_update('figure_24_ucb_1_fp_raw.dat'), workload_latency_query('figure_24_ucb_1_fp_raw.dat')))
    f.write('4  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_24_ub_1_raw.dat'), workload_latency_query('figure_24_ub_1_raw.dat')))
    f.write('5  0         0 ""\n')
    f.write('6  {1} {0} "In-place"\n'.format(workload_latency_update('figure_24_naive_5_raw.dat'), workload_latency_query('figure_24_naive_5_raw.dat')))
    f.write('7  {1} {0} "UCB"\n'.format(workload_latency_update('figure_24_ucb_5_raw.dat'), workload_latency_query('figure_24_ucb_5_raw.dat')))
    f.write('8  {1} {0} "UpBit-FP"\n'.format(workload_latency_update('figure_24_ucb_5_fp_raw.dat'), workload_latency_query('figure_24_ucb_5_fp_raw.dat')))
    f.write('9  {1} {0} "UpBit"\n'.format(workload_latency_update('figure_24_ub_5_raw.dat'), workload_latency_query('figure_24_ub_5_raw.dat')))
    f.write('10  0         0 ""\n')
    f.write('11  {1} {0} "In-place"\n'.format(workload_latency_update('figure_24_naive_10_raw.dat'), workload_latency_query('figure_24_naive_10_raw.dat')))
    f.write('12 {1} {0} "UCB"\n'.format(workload_latency_update('figure_24_ucb_10_raw.dat'), workload_latency_query('figure_24_ucb_10_raw.dat')))
    f.write('13 {1} {0} "UpBit-FP"\n'.format(workload_latency_update('figure_24_ucb_10_fp_raw.dat'), workload_latency_query('figure_24_ucb_10_fp_raw.dat')))
    f.write('14 {1} {0} "UpBit"\n'.format(workload_latency_update('figure_24_ub_10_raw.dat'), workload_latency_query('figure_24_ub_10_raw.dat')))
    f.close()


def gen_figure_25():
    print 'Figure 25'
    print '-' * 10
    dat = '''598362088   "In-place"
    598362184   "UCB"
    598362088   "UpBit-FP"
    598362088   "UpBit"
    0           ""
    1419355200   "In-place"
    1419355200   "UCB"
    1419355200   "UpBit-FP"
    1419355200   "UpBit"
    0           ""
    598376452   "In-place"
    598401844   "UCB"
    598362088   "UpBit-FP"
    592984368   "UpBit"
    0           ""
    1290366000   "In-place"
    1419355200   "UCB"
    1419355200   "UpBit-FP"
    1418065312   "UpBit"
    '''
    f = open('dat/mem_vm.dat', 'w')
    f.write(dat)
    f.close()

def gen_figure_26():
    print 'Figure 26'
    print '-' * 10
    f = open('dat/fp_ub_su.dat', 'w')
    f.write('Title           Read        Update        Memory\n')
    f.write('"length = 10^0" {}          {}            1196723376\n'.format(latency_query('ub_fp_0'), latency_update('ub_fp_0')))
    f.write('"length = 10^1" {}          {}            198467776\n'.format(latency_query('ub_fp_10'), latency_update('ub_fp_10')))
    f.write('"length = 10^2" {}          {}            25054160\n'.format(latency_query('ub_fp_100'), latency_update('ub_fp_100')))
    f.write('"length = 10^3" {}          {}            2573480\n'.format(latency_query('ub_fp_1000'), latency_update('ub_fp_1000')))
    f.write('"length = 10^4" {}          {}            258400\n'.format(latency_query('ub_fp_10000'), latency_update('ub_fp_10000')))
    f.write('"length = 10^5" {}          {}            26400\n'.format(latency_query('ub_fp_100000'), latency_update('ub_fp_100000')))
    f.write('"length = 10^6" {}          {}            2512\n'.format(latency_query('ub_fp_1000000'), latency_update('ub_fp_1000000')))
    f.write('"length = 10^7" {}          {}            112\n'.format(latency_query('ub_fp_10000000'), latency_update('ub_fp_10000000')))
    f.write('"no FP"         {}          {}            0\n'.format(latency_query('ub_fp_no'), latency_update('ub_fp_no')))


def run():
    # Group zero
    # FIXME: use no update as first query
    gen_figure_1()
    gen_figure_9()
    # Group one
    gen_figure_10()
    gen_figure_11()
    gen_figure_12()
    gen_figure_13()
    gen_figure_14()
    gen_figure_15()
    gen_figure_16()
    gen_figure_17()
    gen_figure_18()
    # Group two
    gen_figure_5()
    gen_figure_6()
    # Group three
    gen_figure_19()
    gen_figure_24()
    # Group four
    gen_figure_21()
    # Group five
    gen_figure_20()
    gen_figure_23()
    gen_figure_25()
    gen_figure_26()

def gen_graph():
    os.chdir("gnuplot-scripts")
    os.system("./check_dat_files.sh")
    os.system("./prepare_normalized.sh")
    os.system("make")
    os.chdir("../graphs")
    os.system('echo "Figures generated in \"`pwd`\""')
    os.system('ls -l')

def main():
    os.chdir(ROOT_PATH)
    datdir = 'dat'
    if os.path.exists(datdir) and os.path.isdir(datdir):
        print 'Deleting existing directory ./dat'
        shutil.rmtree(datdir)
    os.mkdir(datdir)
    gen_data()
    build()
    build_index()
    run()
    gen_graph()
    sys.stderr.write('Done!\n')


if __name__ == '__main__':
    main()
