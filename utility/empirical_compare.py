# \author: zhaofeng-shu33

# built-in module
import random
import json
import os
import logging
import pdb
# third party module
import sklearn
from sklearn import metrics
from sklearn import cluster
from sklearn import datasets
from sklearn import preprocessing
from sklearn.neighbors import kneighbors_graph
from sklearn.model_selection import cross_validate
import numpy as np
from tabulate import tabulate
import argparse

# user provided module
import info_cluster
from uci_glass import fetch_uci_glass
from uci_libras import fetch_uci_libras
import schema
# module level global variables go here

logging.basicConfig(filename=os.path.join(schema.BUILD_DIR, schema.EMPIRICAL_LOGGING_FILE), level=logging.INFO, format='%(asctime)s %(message)s')


def _k_means(feature, ground_truth, n_c):
    y_pred_kmeans = cluster.KMeans(n_clusters=n_c['nc']).fit_predict(feature)
    ars_kmeans = metrics.adjusted_rand_score(ground_truth, y_pred_kmeans)    
    return ars_kmeans

def _spectral_clustering(feature, ground_truth, n_c):
    c = cluster.SpectralClustering(n_clusters=n_c['nc'], affinity="nearest_neighbors", eigen_solver='arpack') # construct affinity matrix from rbf kernel function
    # cannot use cv since spectral clustering does not provide fitting method
    y_pred_sc = c.fit_predict(feature)
    sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
    return sc

def _info_clustering(feature, ground_truth, parameter):
    g = info_cluster.InfoCluster(gamma = parameter['gamma'], affinity = parameter['affinity'], n_neighbors = parameter['n_neighbors'])
    y_pred_ic = g.get_category(parameter['nc'], feature)
    sc = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
    return sc

def _affinity_propagation(feature, ground_truth, p_d):
    p = p_d['preference']
    d = p_d['damping_factor']
    if(p_d.get('affinity') and p_d['affinity'] == 'precomputed'):
        connectivity = kneighbors_graph(feature, n_neighbors=p_d['n_neighbors'], include_self=True)
        affinity_matrix = 0.5 * (connectivity + connectivity.T)
        affinity_matrix = np.asarray(affinity_matrix.todense(),dtype=float)
        af = cluster.AffinityPropagation(damping=d, affinity='precomputed').fit(affinity_matrix)
    else:
        af = cluster.AffinityPropagation(preference=p, damping=d).fit(feature)
    y_pred_af = af.labels_
    ars_af = metrics.adjusted_rand_score(ground_truth, y_pred_af)
    return ars_af
    
def _agglomerative(feature, ground_truth, config):
    n_c = config['nc']
    _linkage = config['linkage']
    c = cluster.AgglomerativeClustering(n_clusters=n_c, linkage = _linkage)
    y_pred_sc = c.fit_predict(feature)
    sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
    return sc    
    
def compute_adjusted_rand_score(feature, ground_truth, parameter_dic):
    dic = {}    
    for method, parameter in parameter_dic.items():
        _method = method.replace('-','_')
        exec('ari = _{0}(feature, ground_truth, parameter)'.format(_method))
        _ari = locals()['ari']
        dic[method] = _ari
        if not(parameter.get('ari') and parameter['ari'] > _ari):
            parameter['ari'] = _ari
        logging.info('%s ari is %.3f' % (method, _ari))    
    return dic
    
def Gaussian(parameter_dic):
    data = schema.get_npx('Gaussian.npx')
    if(data is None):
        print("You need to generate Gaussian.npx by running 'python fine_tuning.py' first")
        exit(0)
    pos_list = data[:,:2]
    ground_truth = data[:,-1]
    return compute_adjusted_rand_score(pos_list, ground_truth, parameter_dic)
    
    
def Circle(parameter_dic):
    data = schema.get_npx('Circle.npx')
    if(data is None):
        print("You need to generate Circle.npx by running 'python fine_tuning.py' first")
        exit(0)    
    pos_list = data[:,:2]
    ground_truth = data[:,-1]
    return compute_adjusted_rand_score(pos_list, ground_truth, parameter_dic)
        
def Iris(parameter_dic):
    feature, ground_truth = datasets.load_iris(return_X_y = True)
    return compute_adjusted_rand_score(feature, ground_truth, parameter_dic)    

def Glass(parameter_dic):
    feature, ground_truth = fetch_uci_glass()
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, parameter_dic)

def Libras(parameter_dic):
    feature, ground_truth = fetch_uci_libras()
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, parameter_dic)
    
def compute(use_cloud, dataset_list):
    json_str = schema.get_file(schema.PARAMETER_FILE, use_cloud)
    p_dic = json.loads(json_str)
    dic = {}
    for key in dataset_list:
        logging.info('Start computing for dataset %s'%key)
        exec('dic["{0}"] = {0}(p_dic["{0}"])'.format(key))
    schema.set_file(schema.PARAMETER_FILE, json.dumps(p_dic, indent=4), use_cloud)
    return dic

def make_table(dic, tb_name, method_dic):
    table = [[i] for i in method_dic]
    for i in table:
        for _, v in dic.items():
            i.append('%.2f'%v.get(i[0]))
    _headers = ['adjusted rand index']
    _headers.extend(list(dic.keys()))
    for i in table:
        i[0] = i[0].replace('_', ' ')
    latex_table_string = tabulate(table, headers = _headers, tablefmt = 'latex_raw')
    schema.set_file(tb_name, latex_table_string)
    
if __name__ == '__main__':
    dataset_choices = [i for i in schema.DATASET_SCHEMA]
    method_chocies = [i for i in schema.METHOD_SCHEMA]
    dataset_choices.append('all')
    method_chocies.append('all')
    parser = argparse.ArgumentParser()    
    parser.add_argument('--use_cloud', help='whether to use cloud parameter.json', default=False, type=bool, nargs='?', const=True)
    parser.add_argument('--custom_table_name', help='user provided latex table name instead of schema.LATEX_TABLE_NAME', default=schema.LATEX_TABLE_NAME)
    parser.add_argument('--ignore_computing', help='whether to ignore computing and use ari field in parameter file directly', default=False, type=bool, nargs='?', const=True)    
    parser.add_argument('--debug', help='whether to enter debug mode', default=False, type=bool, nargs='?', const=True)    
    parser.add_argument('--dataset', help='name of the dataset to fine tuning', default='all', choices=dataset_choices, nargs='+')
    parser.add_argument('--method', help='clustering method to fine tuning', default='all', choices=method_chocies, nargs='+')    
    args = parser.parse_args()

    dataset_choices.pop()
    method_chocies.pop()
    dataset_dic = {}
    method_dic = {}
    if(type(args.dataset) is str):
        args.dataset = [args.dataset]
    for i in args.dataset:
        if(i == 'all'):
            for j in dataset_choices:
                dataset_dic[j] = True
            break
        dataset_dic[i] = True
    if(type(args.method) is str):
        args.method = [args.method]        
    for i in args.method:
        if(i == 'all'):
            for j in method_chocies:
                method_dic[j] = True
            break
        method_dic[i] = True

    if(args.debug):
        pdb.set_trace()
    if(args.ignore_computing):
        json_str = schema.get_file(schema.PARAMETER_FILE, args.use_cloud)
        p_dic = json.loads(json_str)
        dic = {}
        for k, v in p_dic.items():
            if(dataset_dic.get(k) is None):
                continue
            dic[k] = {}
            for k_1, v_1 in v.items():
                if(v_1.get('ari')):
                    dic[k][k_1] = v_1['ari']
                else:
                    dic[k][k_1] = 0        
    else:
        dic = compute(args.use_cloud, dataset_dic)
    make_table(dic, args.custom_table_name, method_dic)