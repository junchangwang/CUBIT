from os import listdir, remove, chdir
import os
from os.path import isfile, join, exists
import re
import statistics as stati
import numpy as np
import matplotlib.pyplot as plt

dir = os.getcwd()

y_val_1 = []
y_val_2 = []
x_val = ["10M", "100M", "500M", "1B"]

file_map = {
    "10M"  :   'dat/AVE_naive_latency_10M.dat',
    #"50M"  :   'dat/AVE_naive_latency_50M.dat',
    "100M" :   'dat/AVE_naive_latency_100M.dat',
    "500M" :   'dat/AVE_naive_latency_500M.dat',
    "1B"   :   'dat/AVE_naive_latency_1B.dat',  
}
                
def cal_percentile(data):
    for q in [50, 90, 95, 99, 99.9, 99.99, 99.999, 100]:
        print ("{}% percentile: {}".format (q, np.percentile(data, q)))

def get_figure_data():
    cmd = "python3 eva-scripts/run_naive_latency.py"
    os.system(cmd)  
        
def draw_bar():
    for i in x_val:
        f = open(file_map[i])
        line = f.readline
        for line in f:
            a = line.split()
            y_val_1.append(float(a[0]))
            y_val_2.append(float(a[4]))
        f.close()   
    
    plt.rcParams['font.family'] = ['Linux Libertine O']
    #plt.rcParams['axes.unicode_minus'] = False
    
    #plt.figure(figsize = (10, 8))
    #plt.gcf().subplots_adjust(left = None, right = None, top = None, bottom = 0.01)
    fig, ax = plt.subplots()
    
    x_width = range(0, len(x_val)) 
    x_width2 = [i + 0.35 for i in x_width]  
    
    ax.bar(x_width, y_val_1, lw = 0.8, fc = "black", alpha = 0.8, width = 0.35, label = "Query")
    ax.bar(x_width2, y_val_2, lw = 0.8, fc = "white", edgecolor = "black", alpha = 0.8, width = 0.35, label = "UDI", hatch = '//')
    
    for i, j in zip(x_width, y_val_1):
        ax.text(i, j + 0.05, "%d"%j, ha = "center", va = "bottom", fontsize = '14')
        
    for i, j in zip(x_width2, y_val_2):
        ax.text(i, j + 0.05, "%d"%j, ha = "center", va = "bottom", fontsize = '14')
        
    plt.xticks([i + 0.15 for i in range(len(x_val))], x_val, fontsize = '20')
    plt.yticks(fontsize = '20')
    
    legend = ax.legend(frameon = False, loc = 'upper left', shadow = False, fontsize='20')
    
    plt.xlabel("(b) Dataset entries",fontsize = '25')
    plt.ylabel("Latency (ms)",fontsize = '25')
    
    plt.tight_layout()

    plt.savefig(dir + '/dat/figure/Bar_naive_latency.eps', format='eps', dpi= 1200)

#get_figure_data()   
draw_bar()  
            
