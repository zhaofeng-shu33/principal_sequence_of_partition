# \author: zhaofeng-shu33

# built-in module
import random
import json
import logging
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
LATEX_TABLE_NAME = 'compare.tex'
PARAMETER_FILE_NAME = 'parameter.json'
logging.basicConfig(filename='empirical_compare.log', level=logging.INFO, format='%(asctime)s %(message)s')

def construct_sim_matrix(num_of_points, pos_sim_list):
    '''
    each element of pos_sim_list is a tuple of the form `(pos_x, pos_y, sim_value)`
    '''
    sim_matrix = np.zeros([num_of_points, num_of_points])
    for pos_x, pos_y, sim_value in pos_sim_list:
        sim_matrix[pos_x, pos_y] = sim_value
        sim_matrix[pos_y, pos_x] = sim_value
    return sim_matrix

def _kmeans(feature, ground_truth, n_c):
    y_pred_kmeans = cluster.KMeans(n_clusters=n_c).fit_predict(feature)
    ars_kmeans = metrics.adjusted_rand_score(ground_truth, y_pred_kmeans)    
    return ars_kmeans

def _spectral_clustering(feature, ground_truth, n_c):
    c = cluster.SpectralClustering(n_clusters=n_c, affinity="nearest_neighbors", eigen_solver='arpack') # construct affinity matrix from rbf kernel function
    # cannot use cv since spectral clustering does not provide fitting method
    y_pred_sc = c.fit_predict(feature)
    sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
    return sc

def _info_clustering(feature, ground_truth, n_c):
    g = info_cluster.InfoCluster(gamma = 0.6, affinity = 'nearest_neighbors')
    g.fit(feature)
    y_pred_ic = g.get_category(n_c)
    sc = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
    return sc

def _affinity_propagation(feature, ground_truth, p_d):
    p, d = p_d
    af = cluster.AffinityPropagation(preference=p, damping=d).fit(feature)
    y_pred_af = af.labels_
    ars_af = metrics.adjusted_rand_score(ground_truth, y_pred_af)
    return ars_af
    
def compute_adjusted_rand_score(feature, ground_truth, parameter_dic):
    
    ars_kmeans = _kmeans(feature, ground_truth, parameter_dic['k-means'])
    logging.info('kmeans ari is %.3f' % ars_kmeans)
    
    ars_sc = _spectral_clustering(feature, ground_truth, parameter_dic['spectral clustering'])
    logging.info('spectral clustering ari is %.3f' % ars_sc)
    
    # there are hyperparameters (preference and damping)in AP algorithm, but we don't fine tune it.
    ars_af = _affinity_propagation(feature, ground_truth, parameter_dic['affinity propagation'])
    logging.info('affinity propagation ari is %.3f' % ars_af)
    
    ars_ic = _info_clustering(feature, ground_truth, parameter_dic['info-clustering'])
    logging.info('info clustering ari is %.3f' % ars_ic)
    
    return {'k-means':ars_kmeans, 'spectral clustering':ars_sc, 'affinity propagation': ars_af, 'info-clustering': ars_ic}
    
def Gaussian(parameter_dic):
    data = np.load('Gaussian.npx')
    pos_list = data[:,:2]
    ground_truth = data[:,-1]
    return compute_adjusted_rand_score(pos_list, ground_truth, parameter_dic)
    
    
def Circle(parameter_dic):
    data = np.load('Circle.npx')
    pos_list = data[:,:2]
    ground_truth = data[:,-1]
    return compute_adjusted_rand_score(pos_list, ground_truth, parameter_dic)
        
def Iris(parameter_dic):
    feature, ground_truth = datasets.load_iris(return_X_y = True)
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, parameter_dic)    

def Glass(parameter_dic):
    feature, ground_truth = fetch_uci_glass()
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, parameter_dic)

def Libras(parameter_dic):
    feature, ground_truth = fetch_uci_libras()
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, parameter_dic)
    
def compute():
    global PARAMETER_FILE_NAME
    p_dic = json.loads(open(PARAMETER_FILE_NAME).read())
    dic = {}
    for key in p_dic:
        logging.info('Start computing for dataset %s'%key)
        exec('dic["{0}"] = {0}(p_dic["{0}"])'.format(key))
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
