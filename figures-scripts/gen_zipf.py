import numpy as np
import struct

count = 0
alpha = 1.5
f = open('zipf_15', 'wb')
while True:
    s = np.random.zipf(alpha, 1000000)
    for d in s:
        d = d - 1
        if d >= 100:
            continue
        count += 1
        f.write(struct.pack('i', d))
        if count >= 100000000:
            break
        elif count % 1000000 == 0:
            print count
    if count >= 100000000:
        break
f.close()
