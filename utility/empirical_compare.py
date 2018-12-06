# \author: zhaofeng-shu33
import sklearn
import graph_cluster
from sklearn import metrics
from sklearn import cluster
from sklearn import datasets
from sklearn.model_selection import cross_validate
import numpy as np
import random
NUM_OF_CLUSTER = [3, 4, 5]
def construct_sim_matrix(num_of_points, pos_sim_list):
    '''
    each element of pos_sim_list is a tuple of the form `(pos_x, pos_y, sim_value)`
    '''
    sim_matrix = np.zeros([num_of_points, num_of_points])
    for pos_x, pos_y, sim_value in pos_sim_list:
        sim_matrix[pos_x, pos_y] = sim_value
        sim_matrix[pos_y, pos_x] = sim_value
    return sim_matrix

def _generate_three_circle_data():
    pos_list = []
    num_list = [60,100,140]
    ground_truth = []
    for i in range(1,4): # radius: 0.1*i
        for j in range(num_list[i-1]):
            r = 0.1*i + 0.01 * (2*random.random()-1)
            angle = 2*np.pi * random.random()
            pos_list.append([r * np.cos(angle), r * np.sin(angle)])
            ground_truth.append(i)
    return (pos_list, ground_truth)

def _kmeans(feature, ground_truth):
    global NUM_OF_CLUSTER
    ref_sc = 0
    optimal_n_c = 0
    for n_c in NUM_OF_CLUSTER:
        c = cluster.KMeans(n_clusters=n_c)
        scores = cross_validate(c, feature, ground_truth, scoring='adjusted_rand_score', cv=5, return_train_score=False)
        sc = scores['test_score'].mean()
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    y_pred_kmeans = cluster.KMeans(n_clusters=optimal_n_c).fit_predict(feature)
    ars_kmeans = metrics.adjusted_rand_score(ground_truth, y_pred_kmeans)    
    return ars_kmeans

def _spectral_clustering(feature, ground_truth):
    global NUM_OF_CLUSTER
    ref_sc = 0
    optimal_n_c = 0
    for n_c in NUM_OF_CLUSTER:
        c = cluster.SpectralClustering(n_clusters=n_c, affinity="nearest_neighbors") # construct affinity matrix from rbf kernel function
        # cannot use cv since spectral clustering does not provide fitting method
        y_pred_sc = c.fit_predict(feature)
        sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    return ref_sc

def _info_clustering(feature, ground_truth):
    # we should fine tuning the min required cluster instead of lambda s.t. I(Z_V)>lambda
    # this is because lambda is variant in difference cases, and grid search is not economic
    global NUM_OF_CLUSTER
    ref_sc = 0
    optimal_n_c = 0
    
    g = graph_cluster.InfoCluster(num_points, 0.6, feature)
    g.run()
    y_pred_ic = g.get_category(4)
    ars_ic = metrics.adjusted_rand_score(ground_truth, y_pred_ic)    
    for n_c in NUM_OF_CLUSTER:
        c = cluster.SpectralClustering(n_clusters=n_c, affinity="nearest_neighbors") # construct affinity matrix from rbf kernel function
        # cannot use cv since spectral clustering does not provide fitting method
        y_pred_sc = c.fit_predict(feature)
        sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    return ref_sc
    
def compute_adjusted_rand_score(feature, ground_truth):
    num_points = len(feature)

                
    
    ars_kmeans = _kmeans(feature, ground_truth)
    
    ars_sc = _spectral_clustering(feature, ground_truth)
    
    # there are hyperparameters (preference and damping)in AP algorithm, but we don't fine tune it.
    af = cluster.AffinityPropagation().fit(feature)
    y_pred_af = af.labels_
    ars_af = metrics.adjusted_rand_score(ground_truth, y_pred_af)
    
    g.run()
    y_pred_ic = g.get_category(4)
    ars_ic = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
    
    return {'k-means':ars_kmeans, 'spectral clustering':ars_sc, 'affinity propagation': ars_af, 'info-clustering': ars_ic}
    
def Gaussian():
    pos_list, ground_truth = datasets.make_blobs(n_samples = 100, centers=[[3,3],[-3,-3],[3,-3],[-3,3]], cluster_std=1)
    return compute_adjusted_rand_score(pos_list, ground_truth)
    
    
def Circle():
    '''
    Todo: distance measure should use polar coordinate
    '''
    pos_list, ground_truth = _generate_three_circle_data()
    return compute_adjusted_rand_score(pos_list, ground_truth)
        

    
def compute():
    dic = {}
    dic['Gaussian'] = Gaussian()
    dic['Circle'] = Circle()
    return dic
if __name__ == '__main__':
    print(compute())
