import sys
import os
import shutil

ROOT_PATH = os.getcwd()

TEN_MILLION = str(10000000)
TEN_MILLION_DATA = 'a_' + TEN_MILLION + '_100'
FIFTY_MILLION = str(50000000)
FIFTY_MILLION_DATA = 'a_' + FIFTY_MILLION + '_100'
ONE_HUNDRED_MILLION = str(100000000)
ONE_HUNDRED_MILLION_DATA = 'a_' + ONE_HUNDRED_MILLION + '_100'
FIVE_HUNDRED_MILLION = str(500000000)
FIVE_HUNDRED_MILLION_DATA = 'a_' + FIVE_HUNDRED_MILLION + '_100'
ONE_BILLION = str(1000000000)
ONE_BILLION_DATA = 'a_' + ONE_BILLION + '_100'




def gen_data():
    cmd = 'python generate.py {} {}'
    os.system(cmd.format(TEN_MILLION, 100))
    os.system(cmd.format(FIFTY_MILLION, 100))
    os.system(cmd.format(ONE_HUNDRED_MILLION, 100))
    os.system(cmd.format(FIVE_HUNDRED_MILLION, 100))
    os.system(cmd.format(ONE_BILLION, 100))     

def build():
    cmd = 'sh build.sh'
    os.system(cmd)

def build_index():
    cmd = './build/nicolas -m build -d {} -c {} -n {} -i {} '
    os.system(cmd.format(TEN_MILLION_DATA, 100, TEN_MILLION, '10M_100'))
    os.system(cmd.format(FIFTY_MILLION_DATA, 100, FIFTY_MILLION, '50M_100'))
    os.system(cmd.format(ONE_HUNDRED_MILLION_DATA, 100, ONE_HUNDRED_MILLION, '100M_100'))
    os.system(cmd.format(FIVE_HUNDRED_MILLION_DATA, 100, FIVE_HUNDRED_MILLION, '500M_100'))
    os.system(cmd.format(ONE_BILLION_DATA, 100, ONE_BILLION, '1B_100'))

    
def main():
    gen_data()
    build()
    build_index()
    

if __name__ == '__main__':
    main()    
    
    
    
