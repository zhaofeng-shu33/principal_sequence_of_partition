import json
import os
METHOD_SCHEMA = {
        'k-means': {'nc':[0]},
        'spectral_clustering': {'nc':[0]},
        'affinity_propagation': {
            'preference':[-50],
            'damping_factor':[0.5]
        },
        'info-clustering': {
            'nc':[0],
            'affinity':[ "nearest_neighbors", "rbf" ],
            'n_neighbors':[10],
            'gamma':[0.6]
        }
}
DATASET_SCHEMA = {
  'Gaussian': 4,
  'Circle': 3,
  'Iris': 3,
  'Glass': 6,
  'Libras': 15
}
BUILD_DIR = 'build'
TUNING_FILE = 'tuning.json'
PARAMETER_FILE = 'parameter.json'
def construct_tuning_json():
    '''construct tuning json string
    '''
    global METHOD_SCHEMA, DATASET_SCHEMA
    dic = {}
    for dataset, optimal_nc in DATASET_SCHEMA.items():
        dic[dataset] = {}
        dic_dataset = dic[dataset]
        for method, parameter_dic in METHOD_SCHEMA.items():
                dic_dataset[method] = {}
                dic_dataset_method = dic_dataset[method]
                for parameter, default_value in parameter_dic.items():
                    if(parameter == 'nc'):
                        v = [optimal_nc]
                    else:
                        v = default_value
                    dic_dataset_method[parameter] = v
    return json.dumps(dic, indent=4)                
    
def get_tuning_file():
    '''return tuning json string    
    '''
    tuning_file_path = os.path.join(BUILD_DIR, TUNING_FILE)
    json_str = ''
    if not(os.path.exists(tuning_file_path)):
        with open(tuning_file_path, 'w') as f:
            json_str = construct_tuning_json()
            f.write(json_str)            
    else:
        with open(tuning_file_path, 'r') as f:
            json_str = f.read()
    return json_str
    
if __name__ == '__main__':
    if not(os.path.exists(BUILD_DIR)):
        os.mkdir(BUILD_DIR)
    tuning_file_path = os.path.join(BUILD_DIR, TUNING_FILE)
    tuning_file_construct = False
    if not(os.path.exists(tuning_file_path)):
        tuning_file_construct = True
    else:
        result = input('%s exists, overwrite it?(y/n)'%tuning_file_path)
        if(result == 'y'):
            tuning_file_construct = True
    if(tuning_file_construct):
        print('construct tuning json files...')
        with open(tuning_file_path, 'w') as f:
            json_str = construct_tuning_json()
            f.write(json_str)
        print('tuning files written to %s' % tuning_file_path)
            
        