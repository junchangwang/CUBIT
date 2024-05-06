
from os import listdir, remove, chdir
import os
from os.path import isfile, join, exists
import re
import statistics as stati
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams
plt.rcParams["font.family"] = "Linux Libertine O"

def draw_cdf(data):
    sorted_data = np.sort(data)
    yvals=np.arange(len(sorted_data))/float(len(sorted_data)-1)
    plt.plot(sorted_data, yvals)
    plt.show()

def cal_percentile(data):
    for q in [50, 90, 95, 99, 99.9, 99.99, 99.999, 100]:
        print ("{}% percentile: {}".format (q, np.percentile(data, q)))
        
def check(exp_name):
    with open(exp_name) as f:
        content = f.readlines()
    content = [int(x.strip()) for x in content]
    content = sorted(content)
    print("=========", file_map[exp_name], "=========")
    print("# items:", len(content), "    Min:", min(content), "     Max:", max(content), "    Mean:", stati.mean(content))
    cal_percentile(content)
    print("Extremely large values:")
    for i in range(-10,0):
        print(content[i])
    print("=========  END  =========")

#####################
#####################

experiments_lookup = ['latency_output_cubit-lk',
                      'latency_output_ub',
                      'latency_output_naive',
                      'latency_output_ucb'
                      ]

experiments_lookup_2 = ['latency_output_cubit-lf',   # for ub cubit-lk cubit-lf
                      'latency_output_cubit-lk',
                      'latency_output_ub'
                      ]
# file_map
file_map = {
    'latency_output_naive'   :   'dat_tmp_zipf/figure_naive_latency_raw_w_32_ratio_0.1.dat',
    'latency_output_cubit-lk'    :   'dat_tmp_zipf/figure_cubit-lk_latency_raw_w_32_ratio_0.1.dat',
    'latency_output_cubit-lf'    :   'dat_tmp_zipf/figure_cubit-lf_latency_raw_w_32_ratio_0.1.dat',
    'latency_output_ub'      :   'dat_tmp_zipf/figure_ub_latency_raw_w_32_ratio_0.1.dat',
    'latency_output_ucb'     :   'dat_tmp_zipf/figure_ucb_latency_raw_w_32_ratio_0.1.dat'
}

# name_map
nm_map = {
    'latency_output_naive'   :   'In-place$^{+}$',
    'latency_output_cubit-lk'    :   'CUBIT',
    'latency_output_cubit-lf'    :   'CUBIT-lf$^{+}$',
    'latency_output_ub'      :   'UpBit$^{+}$',
    'latency_output_ucb'     :   'UCB$^{+}$'
}

# linestyle_map
ls_map = {
    'latency_output_naive'   :   (0, (3, 1, 1, 1, 1, 1)),
    'latency_output_cubit-lk'   :   '-',
    'latency_output_cubit-lf'   :   (0, (3, 1, 1, 1)), #'densely dashdotted',
    'latency_output_ub'        :   '-.',
    'latency_output_ucb'         :   'dashed', #'dashdotted',   
    #'HT-Split'      :   
    #'HT-ideal'      :   (0, (3, 1, 1, 1, 1, 1)) #'densely dashdotdotted'
}

# color map
co_map = {
    'latency_output_naive'   :   'black',
    'latency_output_cubit-lk'   :   'b',
    'latency_output_cubit-lf'   :   'b',
    'latency_output_ub'        :   'g',
    'latency_output_ucb'         :   'brown',  
    #'HT-Split'      :   '',
    #'HT-ideal'      :   'black'
}


experiments = experiments_lookup
experiments_2 = experiments_lookup_2
#target = 'nworkers8_avg16_trigger32_pct90_mult2'
dir = os.getcwd() 
print(dir)
mydir = dir #+ '/latency/data'
chdir(mydir)

def figure_Q():
    for exp_name in experiments:
        if exists(exp_name):
            remove(exp_name)
        f = open(file_map[exp_name])
        output = open(exp_name,'w')
        for line in f:
            a = line.split()
            if (len(a) != 2):
                continue
            elif line.startswith('Q ') :
                output.write(str(a[-1]) + '\n')
        f.close()
        output.close()

    for exp_name in experiments:
        check(exp_name)

    fig, ax = plt.subplots()
    for file_name in experiments:
        data = np.loadtxt(file_name)
        sorted_data = np.sort(data)
        yvals=np.arange(len(sorted_data))/float(len(sorted_data)-1)
        sorted_data = np.divide(sorted_data,1000000)
        ax.plot(sorted_data, yvals, label=nm_map[file_name], ls=ls_map[file_name], color=co_map[file_name], lw = '2')

    #plt.xlim(left=0,right=400000)
    plt.ylim(0, 1)
    plt.xlabel("Query Response Time (ms)", fontsize='25')
    plt.ylabel("CDF",fontsize='25')
    legend = ax.legend(loc='lower right', shadow=True, fontsize='25')
    plt.tick_params(labelsize=25)
    plt.tight_layout()
    fig = 1

    if fig == 0:
        plt.show()
    else:
        plt.savefig(dir + '/graphs_zipf/cdf_Q.png', format='png', dpi=1200)
        plt.savefig(dir + '/graphs_zipf/cdf_Q.eps', format='eps', dpi=1200)
    
def figure_UID():
    for exp_name in experiments:
        if exists(exp_name):
            remove(exp_name)
        f = open(file_map[exp_name])
        output = open(exp_name,'w')
        for line in f:
            a = line.split()
            if (len(a) != 2):
                continue
            elif line.startswith('U ') or line.startswith('I ') or line.startswith('D '):
                output.write(str(a[-1]) + '\n')
        f.close()
        output.close()

    for exp_name in experiments:
        check(exp_name)

    fig, ax = plt.subplots()
    for file_name in experiments:
        data = np.loadtxt(file_name)
        sorted_data = np.sort(data)
        yvals=np.arange(len(sorted_data))/float(len(sorted_data)-1)
        sorted_data = np.divide(sorted_data,1000000)
        ax.plot(sorted_data, yvals, label=nm_map[file_name], ls=ls_map[file_name], color=co_map[file_name], lw = '3')

    plt.xlim(left=0,right=3100)
    plt.ylim(0, 1)
    plt.xlabel("UDI latency (ms)", fontsize='28')
    plt.ylabel("CDF", fontsize='28')
    legend = ax.legend(loc='lower right', shadow=True, fontsize='24')
    plt.tick_params(labelsize=28)
    fig = 1
    plt.tight_layout()
    if fig == 0:
        plt.show()
    else:
        plt.savefig(dir + '/graphs_zipf/cdf_UID.eps', format='eps', dpi=1200)
        plt.savefig(dir + '/graphs_zipf/cdf_UID.png', format='png', dpi=1200)

def figure_Q_2():   # for ub cubit cubit-lf
    for exp_name in experiments_2:
        if exists(exp_name):
            remove(exp_name)
        f = open(file_map[exp_name])
        output = open(exp_name,'w')
        for line in f:
            a = line.split()
            if (len(a) != 2):
                continue
            elif line.startswith('Q ') :
                output.write(str(a[-1]) + '\n')
        f.close()
        output.close()

    for exp_name in experiments_2:
        check(exp_name)

    fig, ax = plt.subplots()
    for file_name in experiments_2:
        data = np.loadtxt(file_name)
        sorted_data = np.sort(data)
        yvals=np.arange(len(sorted_data))/float(len(sorted_data)-1)
        sorted_data = np.divide(sorted_data,1000000)
        ax.plot(sorted_data, yvals, label=nm_map[file_name], ls=ls_map[file_name], color=co_map[file_name], lw = '2')

    #plt.xlim(left=0,right=200000)
    plt.ylim(0, 1)
    plt.xlabel("Query Response Time (ms)", fontsize='20')
    plt.ylabel("CDF", fontsize='20')
    legend = ax.legend(loc='lower right', shadow=True, fontsize='20')
    plt.tick_params(labelsize=18)

    fig = 1
    plt.tight_layout()
    if fig == 0:
        plt.show()
    else:
        plt.savefig(dir + '/graphs_zipf/cdf_Q_ub+cubit-lk+cubit-lf.png', format='png', dpi=1200)
        plt.savefig(dir + '/graphs_zipf/cdf_Q_ub+cubit-lk+cubit-lf.eps', format='eps', dpi=1200)
    
def figure_UID_2(): # for ub cubit cubit-lf
    for exp_name in experiments_2:
        if exists(exp_name):
            remove(exp_name)
        f = open(file_map[exp_name])
        output = open(exp_name,'w')
        for line in f:
            a = line.split()
            if (len(a) != 2):
                continue
            elif line.startswith('U ') or line.startswith('I ') or line.startswith('D '):
                output.write(str(a[-1]) + '\n')
        f.close()
        output.close()

    for exp_name in experiments_2:
        check(exp_name)

    fig, ax = plt.subplots()
    for file_name in experiments_2:
        data = np.loadtxt(file_name)
        sorted_data = np.sort(data)
        yvals=np.arange(len(sorted_data))/float(len(sorted_data)-1)
        sorted_data = np.divide(sorted_data,1000000)
        ax.plot(sorted_data, yvals, label=nm_map[file_name], ls=ls_map[file_name], color=co_map[file_name], lw = '2')

    #plt.xlim(left=0,right=70000)
    plt.ylim(0, 1)
    plt.xlabel("UDI latency (ms)", fontsize='20')
    plt.ylabel("CDF", fontsize='20')
    legend = ax.legend(loc='lower right', shadow=True, fontsize='20')
    plt.tick_params(labelsize=18)    
    fig = 1
    plt.tight_layout()
    if fig == 0:
        plt.show()
    else:
        plt.savefig(dir + '/graphs_zipf/cdf_UID_ub+cubit-lk+cubit-lf.png', format='png', dpi=1200)
        plt.savefig(dir + '/graphs_zipf/cdf_UID_ub+cubit-lk+cubit-lf.eps', format='eps', dpi=1200)

print("\n\n\n##################### Q ########################\n\n")     
figure_Q()
print('\n\n##################### UID ########################\n\n')
figure_UID()

# print("\n\n\n##################### Q_ub+cubit-lk+cubit-lf ########################\n\n")     
# figure_Q_2()
# print('\n\n##################### UID_ub+cubit-lk+cubit-lf ########################\n\n')
# figure_UID_2()    
