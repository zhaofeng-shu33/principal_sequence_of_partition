'''
This script is used to collect the plotting data.
'''
import pickle
import os

import numpy as np

from experiment_two_level import evaluate, InfoClusterWrapper

NUM_TIMES = 5
STEP = 10

def collect_z_o_evaluate(alg, z_in_1, z_in_2, z_min, z_max):
    '''z_in_1, z_in_2 are fixed
    '''
    report_list = []
    for z_o in np.linspace(z_min, z_max, num=STEP):
        report = evaluate(NUM_TIMES, alg, z_in_1, z_in_2, z_o)
        report_list.append([z_in_1, z_in_2, z_o, report])
    return report_list

def collect_z_in_2_evaluate(alg, z_in_1, z_o, z_in_2_min, z_in_2_max):
    '''z_in_1, z_in_2 are fixed
    '''
    report_list = []
    for z_in_2 in np.linspace(z_in_2_min, z_in_2_max, num=STEP):
        report = evaluate(NUM_TIMES, alg, z_in_1, z_in_2, z_o)
        report_list.append(report)
    return report_list
    
if __name__ == '__main__':
    ic=InfoClusterWrapper()
    report_list = collect_z_o_evaluate(ic, 14, 2.5, 0.01,0.5)
    f = open(os.path.join('build', 'collect_z_o_evaluate-14-2.5-0.01-0.5.pickle'), 'wb')
    pickle.dump(report_list, f)
    f.close()
