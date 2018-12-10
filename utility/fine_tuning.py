# \author: zhaofeng-shu33
# fine tuning the hyperparameter of differen clustering method
# output it to `parameter.json`
# built-in module
import random
import json
import logging
import argparse
import pdb
# third party module
import sklearn
from sklearn import metrics
from sklearn import cluster
from sklearn import datasets
from sklearn import preprocessing
from sklearn.model_selection import cross_validate
import numpy as np
# user provided module
import info_cluster
from uci_glass import fetch_uci_glass
from uci_libras import fetch_uci_libras

# module level global variables go here
NUM_OF_CLUSTER = [3, 4, 5, 6]
NUM_OF_CLUSTER_LIBRAS = [13, 14, 15, 16]
PREFERENCE_LIST = [-10, -50, -120, -200]
DAMPING_FACTOR_LIST = [0.5, 0.8]
PARAMETER_FILE_NAME = 'parameter.json'
logging.basicConfig(filename='fine_tuning.log', level=logging.INFO, format='%(asctime)s %(message)s')

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
    optimal_n_c = 0
    for n_c in n_clusters_list:
        c = cluster.KMeans(n_clusters=n_c)
        scores = cross_validate(c, feature, ground_truth, scoring='adjusted_rand_score', cv=5, return_train_score=False)
        sc = scores['test_score'].mean()
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    y_pred_kmeans = cluster.KMeans(n_clusters=optimal_n_c).fit_predict(feature)
    ars_kmeans = metrics.adjusted_rand_score(ground_truth, y_pred_kmeans)    
    return optimal_n_c

def _spectral_clustering(feature, ground_truth, n_clusters_list):
    ref_sc = -1
    optimal_n_c = 0
    for n_c in n_clusters_list:
        c = cluster.SpectralClustering(n_clusters=n_c, affinity="nearest_neighbors", eigen_solver='arpack') # construct affinity matrix from rbf kernel function
        # cannot use cv since spectral clustering does not provide fitting method
        y_pred_sc = c.fit_predict(feature)
        sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    return optimal_n_c

def _info_clustering(feature, ground_truth, n_clusters_list):
    # we should fine tuning the min required cluster instead of lambda s.t. I(Z_V)>lambda
    # this is because lambda is variant in difference cases, and grid search is not economic
    ref_sc = -1
    optimal_n_c = 0
    
    g = info_cluster.InfoCluster(gamma = 0.6, affinity = 'nearest_neighbors')
    g.fit(feature)
    for n_c in n_clusters_list:
        y_pred_ic = g.get_category(n_c)
        sc = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    return optimal_n_c
def _affinity_propagation(feature, ground_truth, preference_list, damping_factor_list):
    ref_sc = -1
    optimal_preference = 0
    optimal_damping_factor = -1        
    for p in preference_list:
        for d in damping_factor_list:
            af = cluster.AffinityPropagation(preference=p, damping=d).fit(feature)
            y_pred_af = af.labels_
            ars_af = metrics.adjusted_rand_score(ground_truth, y_pred_af)
            if(ars_af > ref_sc):
                ref_sc = ars_af
                optimal_preference = p
                optimal_damping_factor = d
    return (optimal_preference, optimal_damping_factor)
    
def fine_tuning(feature, ground_truth, n_clusters_list):
    global PREFERENCE_LIST, DAMPING_FACTOR_LIST;
    logging.info('Start tuning for kmeans')
    kmeans_nc = _kmeans(feature, ground_truth, n_clusters_list)
    logging.info('optimal number of cluster is %d'% kmeans_nc)
    
    logging.info('Start tuning for spectral clustering')
    sc_nc = _spectral_clustering(feature, ground_truth, n_clusters_list)
    logging.info('optimal number of cluster is %d' % sc_nc)
    
    # there are two hyperparameters (preference and damping)in AP algorithm.
    logging.info('Start tuning for affinity propagation')    
    af_parameter = _affinity_propagation(feature, ground_truth, PREFERENCE_LIST, DAMPING_FACTOR_LIST)
    logging.info('optimal preference is {0}, optimal damping factor is {1}'.format(af_parameter[0], af_parameter[1]))

    logging.info('Start tuning for info-clustering')        
    ic_nc = _info_clustering(feature, ground_truth, n_clusters_list)
    logging.info('optimal number of cluster is {0}'.format(ic_nc))        
    
    return {'k-means':kmeans_nc, 'spectral clustering':sc_nc, 'affinity propagation': af_parameter, 'info-clustering': ic_nc}
    
def Gaussian():
    global NUM_OF_CLUSTER
    pos_list, ground_truth = datasets.make_blobs(n_samples = 100, centers=[[3,3],[-3,-3],[3,-3],[-3,3]], cluster_std=1)
    return fine_tuning(pos_list, ground_truth, NUM_OF_CLUSTER)
    
    
def Circle():
    global NUM_OF_CLUSTER    
    pos_list, ground_truth = _generate_three_circle_data()
    return fine_tuning(pos_list, ground_truth, NUM_OF_CLUSTER)
        
def Iris():
    global NUM_OF_CLUSTER
    feature, ground_truth = datasets.load_iris(return_X_y = True)
    feature = preprocessing.scale(feature)
    return fine_tuning(feature, ground_truth, NUM_OF_CLUSTER)    

def Glass():
    global NUM_OF_CLUSTER
    feature, ground_truth = fetch_uci_glass()
    feature = preprocessing.scale(feature)
    return fine_tuning(feature, ground_truth, NUM_OF_CLUSTER)

def Libras():
    global NUM_OF_CLUSTER_LIBRAS
    feature, ground_truth = fetch_uci_libras()
    feature = preprocessing.scale(feature)
    return fine_tuning(feature, ground_truth, NUM_OF_CLUSTER_LIBRAS)
    
def compute(dataset):
    global PARAMETER_FILE_NAME
    dic = json.loads(open(PARAMETER_FILE_NAME).read())
    if(dic.get(dataset)):
        return
    logging.info('tuning for dataset ' + dataset)
    exec('dic["%s"] = %s()' % (dataset, dataset)) 
    return dic

def make_json(dic):
    global PARAMETER_FILE_NAME
    open(PARAMETER_FILE_NAME,'w').write(json.dumps(dic))
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('dataset', help='name of the dataset to fine tuning', choices=['Gaussian', 'Circle', 'Iris', 'Glass', 'Libras'])
    args = parser.parse_args()    
    make_json(compute(args.dataset))
