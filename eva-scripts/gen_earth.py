import netCDF4
import pickle as pickle
import struct

nc = netCDF4.Dataset('Land_and_Ocean_EqualArea.nc')

climatology = []
for i in range(12):
    print (i)
    current = []
    for j in range(15984):
        current.append(nc.variables['climatology'][i][j])
    climatology.append(current)

# temperature = []
# mask = []
# for i in range(1980):
#     print (i)
#     current = []
#     m = []
#     for j in range(15984):
#         current.append(nc.variables['temperature'][i][j])
#         m.append(nc.variables['temperature'][i].mask[j])
#     temperature.append(current)
#     mask.append(m)

# data = []
# minval = 100
# for i in range(15984):
#     print (i)
#     row = []
#     for k in range(1980):
#         if nc.variables['temperature'][k].mask[i]:
#             row.append(0)
#             pass
#         else:
#             row.append(int(round(nc.variables['temperature'][k][i] + climatology[k % 12][i])))
#             minval = min(minval, row[-1])
#     data.append(row)
#     print (minval)

data = []
minval = 100
for k in range(1980):
    print (k)
    row = []
    for i in range(15984):
        if str(nc.variables['temperature'][k][i]) == "--":
            row.append(0)
        else:
            row.append(int(round(nc.variables['temperature'][k][i] + climatology[k % 12][i])))
            # minval = min(minval, row[-1])
    data.append(row)
    # print (minval)

# maxval = -100
# minval = 100
# for l in data:
#     maxval = max(maxval, max(l))
#     minval = min(minval, min(l))
# print (maxval, minval)

f = open('a_earth', 'wb')
for i in data:
    for j in i:
        f.write(struct.pack('i', j + 72))
f.close()

exit()


