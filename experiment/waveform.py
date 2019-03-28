# synthesized data cache

# use metric L1 norm
# agglomerative clustering has 4 different linkage type, info-cluster(with dt or with pdt).

# evaluation and compare, time and accuracy
import os
from time import time

import numpy as np
from sklearn.cluster import AgglomerativeClustering
from sklearn.metrics import adjusted_rand_score

from info_cluster import InfoCluster

def generate_data():
    # Generate waveform data
    n_features = 2000
    t = np.pi * np.linspace(0, 1, n_features)

    def sqr(x):
        return np.sign(np.cos(x))

    X = list()
    y = list()
    for i, (phi, a) in enumerate([(.5, .15), (.5, .6), (.3, .2)]):
        for _ in range(30):
            phase_noise = .01 * np.random.normal()
            amplitude_noise = .04 * np.random.normal()
            additional_noise = 1 - 2 * np.random.rand(n_features)
            # Make the noise sparse
            additional_noise[np.abs(additional_noise) < .997] = 0

            X.append(12 * ((a + amplitude_noise)
                     * (sqr(6 * (t + phi + phase_noise)))
                     + additional_noise))
            y.append(i)

    X = np.array(X)
    y = np.array(y)    
    return (X, y)

def get_data():
    if(os.path.exists(os.path.join('build','waveform.npz'))):
        data = np.load(os.path.join('build', 'waveform.npz'))
        X = data['X']
        y = data['y']
    else:
        X, y = generate_data()
        if not(os.path.exists('build')):
            os.mkdir('build')
        np.savez(os.path.join('build','waveform.npz'), X=X, y=y)
    return [X, y]
    
if __name__ == '__main__':
    X, y = get_data()
    # test agglomerative clustering 
    for linkage in ('ward', 'average', 'complete', 'single'):
        if(linkage == 'ward'):
            affinity_method = 'euclidean'
        else:
            affinity_method = 'cityblock'
        ac = AgglomerativeClustering(linkage=linkage, n_clusters=3, affinity=affinity_method)        
        t0 = time()
        ac.fit(X)
        sc = adjusted_rand_score(ac.labels_, y)
        print("%s :\t%.2fs, \t ari: %.2f" % (linkage, time() - t0, sc))
    # test info-clustering
    
