# \author: zhaofeng-shu33
# fine tuning the hyperparameter of differen clustering method
# output it to `parameter.json`
# built-in module
import random
import json
import logging
import argparse
import os
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
TUNING_CONFIGURE_NAME = 'tuning.json'
PARAMETER_FILE_NAME = 'parameter.json'
logging.basicConfig(filename='fine_tuning.log', level=logging.INFO, format='%(asctime)s %(message)s')

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
    return (np.asarray(pos_list), np.asarray(ground_truth))

def _kmeans(feature, ground_truth, config):
    ref_sc = -1
    optimal_n_c = 0
    for n_c in config['n_clusters_list']:
        c = cluster.KMeans(n_clusters=n_c)
        scores = cross_validate(c, feature, ground_truth, scoring='adjusted_rand_score', cv=5, return_train_score=False)
        sc = scores['test_score'].mean()
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    y_pred_kmeans = cluster.KMeans(n_clusters=optimal_n_c).fit_predict(feature)
    ars_kmeans = metrics.adjusted_rand_score(ground_truth, y_pred_kmeans)    
    logging.info('ari %.3f'% ars_kmeans)                
    return optimal_n_c

def _spectral_clustering(feature, ground_truth, config):
    ref_sc = -1
    optimal_n_c = 0
    for n_c in config['n_clusters_list']:
        c = cluster.SpectralClustering(n_clusters=n_c, affinity="nearest_neighbors", eigen_solver='arpack') # construct affinity matrix from rbf kernel function
        # cannot use cv since spectral clustering does not provide fitting method
        y_pred_sc = c.fit_predict(feature)
        sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
        if(sc>ref_sc):
            optimal_n_c = n_c
            ref_sc = sc
    logging.info('ari %.3f'% ref_sc)                        
    return optimal_n_c

def _info_clustering(feature, ground_truth, config):
    # we should fine tuning the min required cluster instead of lambda s.t. I(Z_V)>lambda
    # this is because lambda is variant in difference cases, and grid search is not economic
    ref_sc = -1
    optimal_parameter = {'nc':0,'affinity':'rbf','n_neighbors':0,'gamma':0}
    if(config['affinity'].count('rbf')>0):
        for _gamma in config['gamma_list']:
            g = info_cluster.InfoCluster(gamma = _gamma, affinity = 'rbf')
            for n_c in config['n_clusters_list']:
                y_pred_ic = g.get_category(n_c, feature)
                sc = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
                if(sc>ref_sc):
                    optimal_parameter['affinity'] = 'rbf'
                    optimal_parameter['gamma'] = _gamma
                    optimal_parameter['nc'] = g.get_num_cat(n_c)
                    ref_sc = sc
            logging.info('nc = %d, ari = %.3f, gamma = %f, affinity = rbf'% (optimal_parameter['nc'], sc, _gamma))            
    if(config['affinity'].count('nearest_neighbors')>0):
        for _n_neighbors in config['n_neighbors_list']:
            g = info_cluster.InfoCluster(affinity = 'nearest_neighbors', n_neighbors=_n_neighbors)
            for n_c in config['n_clusters_list']:
                y_pred_ic = g.get_category(n_c, feature)
                sc = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
                if(sc>ref_sc):
                    optimal_parameter['affinity'] = 'nearest_neighbors'
                    optimal_parameter['n_neighbors'] = _n_neighbors
                    optimal_parameter['nc'] = g.get_num_cat(n_c)
                    ref_sc = sc
            logging.info('nc = %d, ari = %.3f, n_neighbors = %d, affinity = nearest_neighbors'% (optimal_parameter['nc'], sc, _n_neighbors))            
            
    return optimal_parameter
    
def _affinity_propagation(feature, ground_truth, config):
    ref_sc = -1
    optimal_preference = 0
    optimal_damping_factor = -1 
    for p in config['preference_list']:
        for d in config['damping_factor_list']:
            af = cluster.AffinityPropagation(preference=p, damping=d).fit(feature)
            y_pred_af = af.labels_
            ars_af = metrics.adjusted_rand_score(ground_truth, y_pred_af)
            if(ars_af > ref_sc):
                ref_sc = ars_af
                optimal_preference = p
                optimal_damping_factor = d
    logging.info('ari %.3f'% ref_sc)                            
    return (optimal_preference, optimal_damping_factor)
    
def fine_tuning(feature, ground_truth, method, config):
    global logging
    dic = {}
    if(method == 'all' or method == 'k-means'):
        logging.info('Start tuning for kmeans')
        parameter = _kmeans(feature, ground_truth, config['k-means'])
        dic['k-means'] = parameter
        logging.info('optimal number of cluster is %d'% parameter)

    if(method == 'all' or method == 'spectral clustering'):        
        logging.info('Start tuning for spectral clustering')
        parameter = _spectral_clustering(feature, ground_truth, config['spectral clustering'])
        dic['spectral clustering'] = parameter        
        logging.info('optimal number of cluster is %d' % parameter)

    if(method == 'all' or method == 'affinity propagation'):                
        # there are two hyper-parameters (preference and damping)in AP algorithm.
        logging.info('Start tuning for affinity propagation')    
        parameter = _affinity_propagation(feature, ground_truth, config['affinity propagation'])
        dic['affinity propagation'] = parameter                
        logging.info('optimal preference is {0}, optimal damping factor is {1}'.format(parameter[0], parameter[1]))
    
    if(method == 'all' or method == 'info-clustering'):        
        logging.info('Start tuning for info-clustering')        
        parameter = _info_clustering(feature, ground_truth, config['info-clustering'])
        dic['info-clustering'] = parameter                
        logging.info('optimal number of cluster is {0}'.format(parameter))        
        
    if(method == 'all'):
        return dic
    else:
        return parameter
        
def Gaussian(method, config):
    if(os.path.exists('Gaussian.npx')):
        data = np.load('Gaussian.npx')
        pos_list = data[:,:2]
        ground_truth = data[:,-1]   
    else:      
        pos_list, ground_truth = datasets.make_blobs(n_samples = 100, centers=[[3,3],[-3,-3],[3,-3],[-3,3]], cluster_std=1)
        np.hstack((pos_list, ground_truth.reshape(len(ground_truth),1))).dump('Gaussian.npx')
    return fine_tuning(pos_list, ground_truth, method, config)
    
    
def Circle(method, config):
    if(os.path.exists('Circle.npx')):
        data = np.load('Circle.npx')
        pos_list = data[:,:2]
        ground_truth = data[:,-1]   
    else:
        pos_list, ground_truth = _generate_three_circle_data()
        np.hstack((pos_list, ground_truth.reshape(len(ground_truth),1))).dump('Circle.npx')    
    return fine_tuning(pos_list, ground_truth, method, config)
        
def Iris(method, config):
    feature, ground_truth = datasets.load_iris(return_X_y = True)
    return fine_tuning(feature, ground_truth, method, config)    

def Glass(method, config):
    feature, ground_truth = fetch_uci_glass()
    feature = preprocessing.scale(feature)
    return fine_tuning(feature, ground_truth, method, config)

def Libras(method, config):
    feature, ground_truth = fetch_uci_libras()
    feature = preprocessing.scale(feature)
    return fine_tuning(feature, ground_truth, method, config)
    
def compute(dataset, method):
    global PARAMETER_FILE_NAME, TUNING_CONFIGURE_NAME, logging
    dic = json.loads(open(PARAMETER_FILE_NAME).read())
    tuning_dic = json.loads(open(TUNING_CONFIGURE_NAME).read())
    logging.info('tuning for dataset ' + dataset)
    config = tuning_dic["%s"%dataset]
    if(method == 'all'):
        exec('dic["{0}"] = {0}("{1}",{2})'.format(dataset, method, config)) 
    else:
        exec('dic["{0}"]["{1}"] = {0}("{1}",{2})' .format(dataset, method, config)) 
    
    return dic

def make_json(dic):
    global PARAMETER_FILE_NAME
    open(PARAMETER_FILE_NAME,'w').write(json.dumps(dic, indent=4))
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('dataset', help='name of the dataset to fine tuning', choices=['Gaussian', 'Circle', 'Iris', 'Glass', 'Libras'])
    parser.add_argument('--method', help='clustering method to fine tuning', default='all', choices=['all', 'k-means', 'spectral clustering', 'affinity propagation', 'info-clustering'])
    args = parser.parse_args()    
    make_json(compute(args.dataset, args.method))
