'''
This script is used to collect the plotting data.
'''
import pickle
import os
import argparse
import pdb

import numpy as np
import datetime

from experiment_two_level import evaluate, InfoClusterWrapper

NUM_TIMES = 5
STEP = 10

def collect_z_o_evaluate(alg, z_in_1, z_in_2, z_min, z_max):
    '''z_in_1, z_in_2 are fixed
    '''
    report_list = []
    for z_o in np.linspace(z_min, z_max, num=STEP):
        report = evaluate(NUM_TIMES, alg, z_in_1, z_in_2, z_o)
        report_list.append(report)
    return report_list

def collect_z_in_2_evaluate(alg, z_in_1, z_o, z_in_2_min, z_in_2_max):
    '''z_in_1, z_in_2 are fixed
    '''
    report_list = []
    for z_in_2 in np.linspace(z_in_2_min, z_in_2_max, num=STEP):
        report = evaluate(NUM_TIMES, alg, z_in_1, z_in_2, z_o)
        report_list.append(report)
    return report_list

def save_to_file(report_list, prefix, *argv):
    time_str = datetime.now().strftime('%Y-%m-%d')
    filename = prefix + '-' + time_str + '-' + argv.join('-') + '.pickle'
    
    f = open(os.path.join('build', filename), 'wb')
    pickle.dump(report_list, f)
    f.close()
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--debug', default=False, type=bool, nargs='?', const=True, help='whether to enter debug mode')
    parser.add_argument('--num_of_times', default=NUM_TIMES, type=int, help='number of times to average each evaluate')
    args = parser.parse_args()
    if(args.debug):
        pdb.set_trace()
    NUM_TIMES = args.num_of_times
    ic=InfoClusterWrapper()
    report_list = collect_z_o_evaluate(ic, 14, 2.5, 0.01, 0.5)
    save_to_file(report_list, 'z_o', 14, 2.5, 0.01, 0.5)