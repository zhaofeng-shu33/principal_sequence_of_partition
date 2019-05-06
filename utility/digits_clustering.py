# try to use info_cluster on 1700 digits (small images)
# The problem is that info_clustering alg is too slow on such large num_of_samples.
import numpy as np
import mahotas
from sklearn.datasets import load_digits
import pdb
from joblib import Memory
from info_cluster import InfoCluster
memory = Memory('./build', verbose=0)

@memory.cache
def func():
    x, y = load_digits(return_X_y=True)
    x1 = np.zeros([x.shape[0], 13])
    for i in range(x.shape[0]):
        a = x[i,:].reshape(8,8).astype('int')
        x1[i,:] = mahotas.features.haralick(a).mean(0)
    return x1
x1 = func()
ic = InfoCluster()
print('starting info-cluster')
ic.fit(x1, use_pdt=True)
