import sys
import collections

def throughput(filename):
    f = open(filename)
    # ff = open(sys.argv[-1] + '.dat', 'w')
    threshold = 100.0
    # threshold = 1000000.0
    sum = 0.0
    n = 0.0
    q = 0.0
    tot = 0
    for line in f:
        a = line.strip().split()
        if len(a) == 0:
            continue
        if a[0] == 'S':
            continue
        elif len(a) == 2:
            tot += 1
            try:
                t = float(a[-1])
            except:
                continue
            if sum + t > threshold:
                if t == 0:
                    t = 1
                n += (threshold - sum) / t
                sum = t - (threshold - sum)
                # ff.write('{}\t{}\n'.format(int(q), float(n) / (10.0)))
                # ff.write('{}\t{}\n'.format(int(q), float(n)))
                q += threshold / 1000000.0
                # q += 1.0
                n = sum / t
            else:
                sum += t
                n += 1
    return float(tot) / (float(q) + float(sum) / threshold)

def latency(filename):
    f = open(filename)
    # ff = open(sys.argv[-1] + '.query.dat', 'w')
    threshold = 5000.0 * 2.0
    sum = 0.0
    n = 0.0
    nn = 0
    q = 0.0
    tot = 0
    vec = []
    for line in f:
        a = line.strip().split()
        if len(a) == 0:
            continue
        if a[0] == 'S':
            continue
        elif len(a) == 2:
            tot += 1
            try:
                t = float(a[-1])
            except:
                continue
            if a[0] == 'Q' or a[0] == 'U':
                vec.append(float(a[-1]))
    return reduce(lambda x, y: x + y, vec) / len(vec)

def throughput_query(filename):
    f = open(filename)
    # ff = open(sys.argv[-1] + '.query.dat', 'w')
    threshold = 500000.0 * 2.0
    sum = 0.0
    n = 0.0
    nn = 0
    q = 0.0
    tot = 0
    vec = []
    for line in f:
        a = line.strip().split()
        if len(a) == 0:
            continue
        if a[0] == 'S':
            continue
        elif len(a) == 2:
            tot += 1
            try:
                t = float(a[-1])
            except:
                continue
            if sum + t > threshold:
                sum = t - (threshold - sum)
                # ff.write('{}\t{}\n'.format(int(q), 0.1 / (n / float(nn) / threshold)))
                vec.append(0.1 / (n / float(nn) / threshold))
                q += threshold / 1000000.0
                n = 0
                nn = 0
            else:
                sum += t
                if a[0] == 'Q':
                    n += float(a[-1])
                    nn += 1
    return reduce(lambda x, y: x + y, vec) / len(vec)

def latency_query(filename):
    f = open(filename)
    # ff = open(sys.argv[-1] + '.query.dat', 'w')
    threshold = 5000.0 * 2.0
    sum = 0.0
    n = 0.0
    nn = 0
    q = 0.0
    tot = 0
    vec = []
    for line in f:
        a = line.strip().split()
        if len(a) == 0:
            continue
        if a[0] == 'S':
            continue
        elif len(a) == 2:
            tot += 1
            try:
                t = float(a[-1])
            except:
                continue
            if a[0] == 'Q':
                vec.append(float(a[-1]))
    return reduce(lambda x, y: x + y, vec) / len(vec)

def workload_latency_query(filename):
    f = open(filename)
    vec = []
    for line in f:
        a = line.strip().split()
        if len(a) == 0:
            continue
        if a[0] == 'S':
            continue
        elif len(a) == 2:
            try:
                t = float(a[-1])
            except:
                continue
            if a[0] == 'Q':
                vec.append(float(a[-1]))
    return sum(vec)

def throughput_update(filename):
    f = open(filename)
    # ff = open(sys.argv[-1] + '.update.dat', 'w')
    threshold = 5000000.0 * 2.0
    sum = 0.0
    n = 0.0
    nn = 0
    q = 0.0
    tot = 0
    vec = []
    for line in f:
        a = line.strip().split()
        if len(a) == 0:
            continue
        if a[0] == 'S':
            continue
        elif len(a) == 2:
            tot += 1
            try:
                t = float(a[-1])
            except:
                continue
            if sum + t > threshold:
                sum = t - (threshold - sum)
                try:
                    # ff.write('{}\t{}\n'.format(int(q), 0.1 / (n / float(nn) / threshold)))
                    vec.append(0.1 / (n / float(nn) / threshold))
                except:
                    pass
                q += 5 * 2.0
                n = 0
                nn = 0
            else:
                sum += t
                if a[0] == 'U' or a[0] == 'D' or a[0] == 'I':
                    n += float(a[-1])
                    nn += 1
    return reduce(lambda x, y: x + y, vec) / len(vec)

def latency_update(filename):
    f = open(filename)
    # ff = open(sys.argv[-1] + '.query.dat', 'w')
    threshold = 5000.0 * 2.0
    sum = 0.0
    n = 0.0
    nn = 0
    q = 0.0
    tot = 0
    vec = []
    for line in f:
        a = line.strip().split()
        if len(a) == 0:
            continue
        if a[0] == 'S':
            continue
        elif len(a) == 2:
            tot += 1
            try:
                t = float(a[-1])
            except:
                continue
            if a[0] == 'U':
                vec.append(float(a[-1]))
    return reduce(lambda x, y: x + y, vec) / len(vec)

def workload_latency_update(filename):
    f = open(filename)
    # ff = open(sys.argv[-1] + '.query.dat', 'w')
    vec = []
    for line in f:
        a = line.strip().split()
        if len(a) == 0:
            continue
        if a[0] == 'S':
            continue
        elif len(a) == 2:
            try:
                t = float(a[-1])
            except:
                continue
            if a[0] == 'U' or a[0] == 'D' or a[0] == 'I':
                vec.append(float(a[-1]))
    return sum(vec)

def mem():
    f = open(sys.argv[-1])
    ff = open(sys.argv[-1] + '.dat', 'w')
    threshold = 1000000.0
    sum = 0.0
    n = 0.0
    q = 0.0
    tot = 0
    M = 0
    for line in f:
        a = line.strip().split()
        if a[0] == 'M':
            M = a[-1]
            continue
        tot += 1
        t = float(a[-1])
        if sum + t > threshold:
            n += (threshold - sum) / t
            sum = t - (threshold - sum)
            ff.write('{}\t{}\n'.format(int(q), M))
            q += 1
            n = sum / t
        else:
            sum += t
            n += 1
    print float(tot) / (float(q) + float(t) / threshold)


def CM():
    f = open(sys.argv[-1])
    ff = open(sys.argv[-1] + '.cm.dat', 'w')
    threshold = 1000.0
    sum = 0.0
    n = 0.0
    q = 0.0
    tot = 0
    vec = []
    for line in f:
        a = line.strip().split()
        if len(a) == 0:
            continue
        if a[0] == 'S':
            continue
        elif len(a) == 2:
            tot += 1
            try:
                t = float(a[-1])
            except:
                continue
            if a[0] == 'PERF':
                n += float(a[-1])
                continue
            if sum > threshold:
                sum += 1.0
                ff.write('{}\t{}\n'.format(int(q), n / threshold))
                vec.append(n / threshold)
                q += threshold
                sum = 0
                n = 0
            else:
                sum += 1.0

    print reduce(lambda x, y: x + y, vec) / len(vec)

def _get(f, str):
    q = collections.deque()
    u = 0
    flag = True
    ans = []
    i, j = 0, 0
    for line in f:
        if line.startswith(str):
            a = line.split()
            if str.startswith('U'):
                i += int(a[-1])
                j += 1
                if j == 2:
                    q.append(i)
                    i, j = 0, 0
                if len(q) > 10:
                    q.popleft()
                continue
            else:
                q.append(int(a[-1]))
                if len(q) > 10:
                    q.popleft()
                continue
        if line.startswith('U ') and len(line.split()) == 2:
            u += 1
            if u == 100 or u == 1010 or u == 10010 or u == 200010 or u == 400010:
                ans.append(reduce(lambda x, y: x + y, q) / len(q))
    print u, ans
    return ans

def get_ud(fn):
    # U EB Decode
    return _get(open(fn), 'U EB Decode')

def get_eb(fn):
    # U EB Decode
    return _get(open(fn), 'U EB Update')

def get_qd(fn):
    return _get(open(fn), 'Q Dec')

def get_qa(fn):
    return _get(open(fn), 'Q And')

if __name__ == '__main__':
    print latency_query(sys.argv[-1])
    # CM()
    # throughput_query()
    # throughput_update()
    # mem()
