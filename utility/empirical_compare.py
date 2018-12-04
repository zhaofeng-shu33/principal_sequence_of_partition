# \author: zhaofeng-shu33
import sklearn
import graph_cluster
from sklearn import metrics
from sklearn import cluster
from sklearn.datasets.samples_generator import make_blobs
import numpy as np
def construct_sim_matrix(num_of_points, pos_sim_list):
    '''
    each element of pos_sim_list is a tuple of the form `(pos_x, pos_y, sim_value)`
    '''
    sim_matrix = np.zeros([num_of_points, num_of_points])
    for pos_x, pos_y, sim_value in pos_sim_list:
        sim_matrix[pos_x, pos_y] = sim_value
        sim_matrix[pos_y, pos_x] = sim_value
    return sim_matrix
def Gaussian():
    pos_list, ground_truth = make_blobs(n_samples = 100, centers=[[3,3],[-3,-3],[3,-3],[-3,3]], cluster_std=1)
    g = graph_cluster.GraphCluster(100, 0.6, pos_list)
                
    y_pred_kmeans = cluster.KMeans(n_clusters=4).fit_predict(g.pos_list)
    ars_kmeans = metrics.adjusted_rand_score(ground_truth, y_pred_kmeans)
    
    sim_matrix = construct_sim_matrix(100, g.pos_sim_list)
    y_pred_sc = cluster.spectral_clustering(sim_matrix, n_clusters=4)
    ars_sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
    
    g.run()
    y_pred_ic = g.get_category(4)
    ars_ic = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
    print(ars_kmeans, ars_sc, ars_ic)
        
if __name__ == '__main__':
    Gaussian()
