'''
This script is used to collect the plotting data.
'''
import pickle
import os
import argparse
import pdb

import numpy as np
from datetime import datetime

from experiment_two_level import evaluate, InfoClusterWrapper

NUM_TIMES = 5
STEP = 10

def collect_z_o_evaluate(alg, z_in_1, z_in_2, z_min, z_max):
    '''z_in_1, z_in_2 are fixed
    '''
    print('evaluate z_o while fixing z_in_1 = %.2f and z_in_2 = %.2f' %(z_in_1, z_in_2))        
    report_list = []
    for z_o in np.linspace(z_min, z_max, num=STEP):
        report = evaluate(NUM_TIMES, alg, z_in_1, z_in_2, z_o)
        report_list.append(report)
    return report_list

def collect_z_in_2_evaluate(alg, z_in_1, z_o, z_in_2_min, z_in_2_max):
    '''z_in_1, z_o are fixed
    '''
    print('evaluate z_in_2 while fixing z_in_1 = %.2f and z_o = %.2f' %(z_in_1, z_o))    
    report_list = []
    for z_in_2 in np.linspace(z_in_2_min, z_in_2_max, num=STEP):
        report = evaluate(NUM_TIMES, alg, z_in_1, z_in_2, z_o)
        report_list.append(report)
    return report_list

def collect_z_in_1_evaluate(alg, z_in_2, z_o, z_in_1_min, z_in_1_max):
    '''z_in_2, z_o are fixed
    '''
    print('evaluate z_in_1 while fixing z_in_2 = %.2f and z_o = %.2f' %(z_in_2, z_o))
    report_list = []
    for z_in_1 in np.linspace(z_in_1_min, z_in_1_max, num=STEP):
        report = evaluate(NUM_TIMES, alg, z_in_1, z_in_2, z_o)
        report_list.append(report)
    return report_list
    
def save_to_file(report_list, prefix, *argv):
    time_str = datetime.now().strftime('%Y-%m-%d')
    filename_middle = ''
    for i in argv:
        filename_middle += '-' + str(i)
    filename = prefix + '-' + time_str + filename_middle + '.pickle'
    
    f = open(os.path.join('build', filename), 'wb')
    pickle.dump(report_list, f)
    f.close()
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    mode_choices = ['z_in_1', 'z_in_2', 'z_o']
    parser.add_argument('--debug', default=False, type=bool, nargs='?', const=True, help='whether to enter debug mode')
    parser.add_argument('--num_of_times', default=NUM_TIMES, type=int, help='number of times to average each evaluate')
    parser.add_argument('--mode', default='z_in_1', choices=mode_choices, help='in which mode to generate plotting data')    
    parser.add_argument('d1', type=float, help='first input value')    
    parser.add_argument('d2', type=float, help='second input value')    
    parser.add_argument('d3', type=float, help='third input value')    
    parser.add_argument('d4', type=float, help='fourth input value')        
    args = parser.parse_args()
    if(args.debug):
        pdb.set_trace()
    NUM_TIMES = args.num_of_times
    ic=InfoClusterWrapper()
    if(args.mode == 'z_in_1'):
        report_list = collect_z_in_1_evaluate(ic, args.d1, args.d2, args.d3, args.d4)
    elif(args.mode == 'z_in_2'):
        report_list = collect_z_in_2_evaluate(ic, args.d1, args.d2, args.d3, args.d4)
    else:
        report_list = collect_z_o_evaluate(ic, args.d1, args.d2, args.d3, args.d4)
        
    save_to_file(report_list, args.mode, NUM_TIMES, args.d1, args.d2, args.d3, args.d4)