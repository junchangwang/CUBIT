import os

bind = map(str, [0, 4, 8, 12, 16, 20, 24, 28,  1, 5, 9, 13, 17, 21, 25, 29, 2, 6, 10, 14, 18, 22, 26, 30, 3, 7, 11, 15, 19, 23, 27, 31] + range(33, 64))
ucb_cmd = 'numactl --physcpubind={} ./build/nicolas -a {} -m update --number-of-queries {} -r {} -c 100 -i /home/yan/codes/nicolas/100M/ -n 100000000 -v yes --fence-pointer yes --num-threads {} --fence-length 10000000 > {}_{}_{}'
ub_cmd = 'numactl --physcpubind={} ./build/nicolas -a {} -m update --number-of-queries {} -r {} -c 100 -i /home/yan/codes/nicolas/100M/ -n 100000000 -v yes --fence-pointer yes --fence-length 1000 --num-threads {} > {}_{}_{}'
naive_cmd = 'numactl --physcpubind={} ./build/nicolas -a {} -m update --number-of-queries {} -r {} -c 100 -i /home/yan/codes/nicolas/100M/ -n 100000000 -v yes --fence-pointer no --num-threads {} > {}_{}_{}'
#curt_cmd = 'perflock numactl --physcpubind={} ./build/nicolas -a {} -m update --number-of-queries {} -r {} -c 4 -i /home/yan/codes/nicolas/1B_4/ -n 1000000000 -v yes --fence-pointer yes --fence-length 10000 --num-threads {}'

for nThread in [2, 4, 8, 16, 32, 64]:
#    for pUpdate in [0.01, 0.05, 0.1]:
    for pUpdate in [0.5]:
        for appr in ['ub', 'naive', 'ucb']:
            curt_bind = ','.join(bind[0:nThread])
            if appr == 'ub':
                curt = ub_cmd.format(curt_bind, appr, int(10000*(1-pUpdate)), int(10000*pUpdate), nThread-1, appr, pUpdate, nThread)
            elif appr == 'ucb':
                curt = ucb_cmd.format(curt_bind, appr, int(10000*(1-pUpdate)), int(10000*pUpdate), nThread-1, appr, pUpdate, nThread)
            else:
                curt = naive_cmd.format(curt_bind, appr, int(10000*(1-pUpdate)), int(10000*pUpdate), nThread-1, appr, pUpdate, nThread)
            print curt
            try:
                os.system(curt)
            except KeyboardInterrupt:
                print 'ctrl-c'
                exit(-1)
