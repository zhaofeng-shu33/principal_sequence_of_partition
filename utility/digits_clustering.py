import numpy as np
import mahotas
from sklearn.datasets import load_digits
import pdb

from info_cluster import InfoCluster

x, y = load_digits(return_X_y=True)
x1 = np.zeros([x.shape[0], 13])
for i in range(x.shape[0]):
    a = x[i,:].reshape(8,8).astype('int')
    x1[i,:] = mahotas.features.haralick(a).mean(0)

ic = InfoCluster()
print('starting info-cluster')
ic.fit(x1)
