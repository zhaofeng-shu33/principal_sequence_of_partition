# \author: zhaofeng-shu33

# built-in module
import random

# third party module
import sklearn
from sklearn import metrics
from sklearn import cluster
from sklearn import datasets
from sklearn import preprocessing
from sklearn.model_selection import cross_validate
import numpy as np
from tabulate import tabulate

# user provided module
import info_cluster
from uci_glass import fetch_uci_glass
from uci_libras import fetch_uci_libras

# module level global variables go here
NUM_OF_CLUSTER = [3, 4, 5, 6]
NUM_OF_CLUSTER_LIBRAS = [13, 14, 15, 16]
LATEX_TABLE_NAME = 'compare.tex'

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
    return (np.asarray(pos_list), ground_truth)

def _kmeans(feature, ground_truth, n_clusters_list):
    ref_sc = -1
    optimal_n_c = -1
    for n_c in n_clusters_list:
        c = cluster.KMeans(n_clusters=n_c)
        scores = cross_validate(c, feature, ground_truth, scoring='adjusted_rand_score', cv=5, return_train_score=False)
        sc = scores['test_score'].mean()
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    y_pred_kmeans = cluster.KMeans(n_clusters=optimal_n_c).fit_predict(feature)
    ars_kmeans = metrics.adjusted_rand_score(ground_truth, y_pred_kmeans)    
    return ars_kmeans

def _spectral_clustering(feature, ground_truth, n_clusters_list):
    ref_sc = 0
    optimal_n_c = -1
    for n_c in n_clusters_list:
        c = cluster.SpectralClustering(n_clusters=n_c, affinity="nearest_neighbors") # construct affinity matrix from rbf kernel function
        # cannot use cv since spectral clustering does not provide fitting method
        y_pred_sc = c.fit_predict(feature)
        sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    return ref_sc

def _info_clustering(feature, ground_truth, n_clusters_list):
    # we should fine tuning the min required cluster instead of lambda s.t. I(Z_V)>lambda
    # this is because lambda is variant in difference cases, and grid search is not economic
    ref_sc = 0
    optimal_n_c = 0
    
    g = info_cluster.InfoCluster(gamma = 0.6)
    g.fit(feature)
    for n_c in n_clusters_list:
        y_pred_ic = g.get_category(n_c)
        sc = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    return ref_sc
    
def compute_adjusted_rand_score(feature, ground_truth, n_clusters_list):
    num_points = len(feature)

                
    
    ars_kmeans = _kmeans(feature, ground_truth, n_clusters_list)
    
    ars_sc = _spectral_clustering(feature, ground_truth, n_clusters_list)
    
    # there are hyperparameters (preference and damping)in AP algorithm, but we don't fine tune it.
    af = cluster.AffinityPropagation().fit(feature)
    y_pred_af = af.labels_
    ars_af = metrics.adjusted_rand_score(ground_truth, y_pred_af)
    
    ars_ic = _info_clustering(feature, ground_truth, n_clusters_list)
    
    return {'k-means':ars_kmeans, 'spectral clustering':ars_sc, 'affinity propagation': ars_af, 'info-clustering': ars_ic}
    
def Gaussian():
    global NUM_OF_CLUSTER
    pos_list, ground_truth = datasets.make_blobs(n_samples = 100, centers=[[3,3],[-3,-3],[3,-3],[-3,3]], cluster_std=1)
    return compute_adjusted_rand_score(pos_list, ground_truth, NUM_OF_CLUSTER)
    
    
def Circle():
    '''
    Todo: distance measure should use polar coordinate
    '''
    global NUM_OF_CLUSTER    
    pos_list, ground_truth = _generate_three_circle_data()
    return compute_adjusted_rand_score(pos_list, ground_truth, NUM_OF_CLUSTER)
        
def Iris():
    global NUM_OF_CLUSTER
    feature, ground_truth = datasets.load_iris(return_X_y = True)
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, NUM_OF_CLUSTER)    

def Glass():
    global NUM_OF_CLUSTER
    feature, ground_truth = fetch_uci_glass()
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, NUM_OF_CLUSTER)

def Libras():
    global NUM_OF_CLUSTER_LIBRAS
    feature, ground_truth = fetch_uci_libras()
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, NUM_OF_CLUSTER_LIBRAS)
    
def compute():
    dic = {}
    dic['Gaussian'] = Gaussian()
    dic['Circle'] = Circle()
    dic['Iris'] = Iris()
    dic['Glass'] = Glass()
    dic['Libras'] = Libras()
    return dic

def make_table(dic):
    global LATEX_TABLE_NAME
    table = [['k-means'],['spectral clustering'],['affinity propagation'],['info-clustering']]
    for i in table:
        for _, v in dic.items():
            i.append('%.2f'%v.get(i[0]))
    _headers = ['']
    _headers.extend(list(dic.keys()))
    latex_table_string = tabulate(table, headers = _headers, tablefmt = 'latex_raw')
    open(LATEX_TABLE_NAME,'w').write(latex_table_string)
    
if __name__ == '__main__':
    make_table(compute())
