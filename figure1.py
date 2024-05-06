
from os import listdir, remove, chdir
import os
from os.path import isfile, join, exists
import re
import statistics as stati
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
from matplotlib import rcParams
import codecs

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
    print("=========", file_map_1[exp_name], "=========")
    print("# items:", len(content), "    Min:", min(content), "     Max:", max(content), "    Mean:", stati.mean(content))
    cal_percentile(content)
    print("Extremely large values:")
    for i in range(-10,0):
        print(content[i])
    print("=========  END  =========")

#####################
#####################

experiments_lookup_1= [ 
                      'latency_output_cubit-lk',
                      'latency_output_ub',
                      'latency_output_naive',
                      'latency_output_ucb'
                      ]

experiments_lookup_2 = [ 
                      'throughputput_cubit-lk',
                      'throughputput_ub',
                      'throughputput_naive',
                      'throughputput_ucb'
                      ]

# file_map
# file_map_1 = {
#     'latency_output_naive'   :   'dat_tmp/figure_naive_latency_raw_w_32_ratio_0.1_c_32.dat',
#     'latency_output_cubit-lk'    :   'dat_tmp/figure_cubit-lk_latency_raw_w_32_ratio_0.1_c_32.dat',
#     'latency_output_ub'      :   'dat_tmp/figure_ub_latency_raw_w_32_ratio_0.1_c_32.dat',
#     'latency_output_ucb'     :   'dat_tmp/figure_ucb_latency_raw_w_32_ratio_0.1_c_32.dat'
# }
file_map_1 = {
     'latency_output_naive'   :   'dat_tmp/figure_naive_latency_raw_w_16_ratio_0.1_c_32.dat',
     'latency_output_cubit-lk'    :   'dat_tmp/figure_cubit-lk_latency_raw_w_16_ratio_0.1_c_32.dat',
     'latency_output_ub'      :   'dat_tmp/figure_ub_latency_raw_w_16_ratio_0.1_c_32.dat',
     'latency_output_ucb'     :   'dat_tmp/figure_ucb_latency_raw_w_16_ratio_0.1_c_32.dat'
}

file_map_2 = {
    # 'throughput_cubit-lf'         :    'dat/figure_cubit-lf_throughput_core.dat',
     'throughputput_cubit-lk'     :    'dat/figure_cubit-lk_throughput_core.dat',
     'throughputput_ub'              :    'dat/figure_ub_throughput_core.dat',
     'throughputput_ucb'            :    'dat/figure_ucb_throughput_core.dat',
     'throughputput_naive'         :    'dat/figure_naive_throughput_core.dat'
}

# name_map
nm_map_1 = {
    'latency_output_naive'   :   'In-place',
    'latency_output_cubit-lk'    :   'Our approach',
    'latency_output_ub'      :   'UpBit',
    'latency_output_ucb'     :   'UCB'
}

nm_map_2 = {
    'throughputput_cubit-lk'     :    'Our approach',
    'throughputput_ub'              :    'UpBit',
    'throughputput_ucb'            :    'UCB',
    'throughputput_naive'         :    'In-place'
}


# linestyle_map
ls_map_1 = {
    'latency_output_naive'   :   (0, (3, 1, 1, 1, 1, 1)),
    'latency_output_cubit-lk'   :   '-',
    'latency_output_ub'        :   '-.',
    'latency_output_ucb'         :   'dashed',  
}

ls_map_2 ={
    'throughputput_naive'         :    (0, (3, 1, 1, 1, 1, 1)),
    'throughputput_cubit-lk'     :    '-',
    'throughputput_ub'              :    '-.',
    'throughputput_ucb'         :   'dashed',  
}

# color map
co_map_1 = {
    'latency_output_naive'   :   'black',
    'latency_output_cubit-lk'   :   'b',
    'latency_output_ub'        :   'g',
    'latency_output_ucb'         :   'brown',  
}

co_map_2 = {
    'throughputput_cubit-lk'     :    'blue',
    'throughputput_ub'              :    'green',
    'throughputput_ucb'            :    'brown', 
    'throughputput_naive'         :    'black',
}

line_marker = {
    'throughputput_cubit-lk'     :    'o',
    'throughputput_ub'          :    'v',
    'throughputput_ucb'         :    '^',
    'throughputput_naive'       :    'd',

}


experiments_1 = experiments_lookup_1
experiments_2 = experiments_lookup_2
dir = os.getcwd() 
print(dir)
mydir = dir #+ '/latency/data'
chdir(mydir)

def figure():
    for exp_name in experiments_1:
        if exists(exp_name):
            remove(exp_name)
        f = open(file_map_1[exp_name])
        output = open(exp_name,'w')
        for line in f:
            a = line.split()
            if (len(a) != 2):
                continue
            elif line.startswith('U ') or line.startswith('I ') or line.startswith('D '):
                output.write(str(a[-1]) + '\n')
        f.close()
        output.close()

    #for exp_name in experiments_1:
    #    check(exp_name)
        
    plt.figure(figsize =(10,3.1))
    plt.subplots_adjust(top = 0.96,bottom = 0.215,right = 0.99,left = 0.095, wspace = 0.25, hspace = 0)
    plt.margins(0, 0)

    # plt.tick_params(labelsize=16)
    ax2 = plt.subplot(1, 2, 1)
    ax1 = plt.subplot(1, 2, 2)
    ax1.tick_params(labelsize=18)
    ax2.tick_params(labelsize=18)
    
    for file_name in experiments_1:
        data = np.loadtxt(file_name)     
        sorted_data = np.sort(data)
        yvals=np.arange(len(sorted_data))/float(len(sorted_data)-1)
        sorted_data2 = np.divide(sorted_data,1000000)
        ax1.plot(sorted_data2, yvals, label=nm_map_1[file_name], ls=ls_map_1[file_name], color=co_map_1[file_name], lw = 2)
        
    # ax2.set_xticks([0,1,2,4,8,16,32],['0','1','2','4','8','16','32'])
    #ax1.set_xlim(0,37)
    ax1.set_ylim(0,1)
    ax1.set_xlabel("Update latency (ms)",fontsize = '20')
    ax1.set_ylabel("CDF",fontsize = '20')
    legend = ax1.legend(loc='lower right', shadow=True, fontsize='15',frameon = False)   
    ax1.text(4800,0.88,'(b)', fontsize=20,fontweight = 'bold')
    for file_name in experiments_2:
        Throughput = []
        Noc = []
        f = open(file_map_2[file_name],"r")
        line = f.readline
        for line in f:
            a =  line.split()
            b = int(a[0])
            c = int(float(a[1]))
            Noc.append(b)
            Throughput.append(c)
        f.close()
        
        ax2.plot(Noc, Throughput, label=nm_map_2[file_name], ls=ls_map_2[file_name], color=co_map_2[file_name], marker = line_marker[file_name], mfc = 'w', ms = 11, lw = '2')
        # ax2.plot(Noc, Throughput, label=nm_map_2[file_name], ls=ls_map_2[file_name], color=co_map_2[file_name], lw = '2')

    #plt.xlim(left=0,right=35)
    ax2.set_xticks([1,4,8,16,24,32])
    ax2.set_ylim(0,2000)
    ax2.set_xlabel("Number of cores",fontsize = '20')
    ax2.set_ylabel("Throughput (op/s)",fontsize = '20')
    ax2.text(28,2000*0.88,'(a)', fontsize=20,fontweight = 'bold')
    legend = ax2.legend(loc='upper left', shadow=True, fontsize='15',frameon = False)
   
   
    fig = 1

    if fig == 0:
        plt.show()
    else:
        plt.savefig(dir + '/graphs/figure1.png', format='png')
        plt.savefig(dir + '/graphs/figure1.eps', format='eps')
    
    
    
    
print('\n\n######################figure()###########################\n\n')
figure()

