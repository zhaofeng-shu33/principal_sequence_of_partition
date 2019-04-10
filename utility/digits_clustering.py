import numpy as np
import mahotas
from sklearn.datasets import load_digits
import pdb
x, y = load_digits(return_X_y=True)
x1 = np.zeros([x.shape[0], 13])
for i in range(x.shape[0]):
    a = x[i,:].reshape(8,8).astype('int')
    x1[i,:] = mahotas.features.haralick(a).mean(0)

from info_cluster import InfoCluster
ic=InfoCluster()
print('starting info-cluster')
ic.fit(x1, use_pdt=True)
pdb.set_trace()