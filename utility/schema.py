import json
import os
import oss2
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
LATEX_TABLE_NAME = 'compare.tex'
EMPIRICAL_LOGGING_FILE = 'empirical_compare.log'
FINE_TUNING_LOGGING_FILE = 'fine_tuning.log'
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
    
def upload_to_my_oss(json_str, file_name):
    global logging
    access_key_id = os.getenv('AccessKeyId')
    access_key_secret = os.getenv('AccessKeySecret')
    if(access_key_secret is not None):
        auth = oss2.Auth(access_key_id, access_key_secret)
        bucket = oss2.Bucket(auth, 'http://oss-cn-shenzhen.aliyuncs.com', 'programmierung')
        research_base = 'research/info-clustering/code/utility/'
        bucket.put_object(research_base + file_name, json_str)

def download_from_my_oss(file_name):
    global logging
    access_key_id = os.getenv('AccessKeyId')
    access_key_secret = os.getenv('AccessKeySecret')
    if(access_key_secret is not None):
        auth = oss2.Auth(access_key_id, access_key_secret)
        bucket = oss2.Bucket(auth, 'http://oss-cn-shenzhen.aliyuncs.com', 'programmierung')
        research_base = 'research/info-clustering/code/utility/'
        file_obj = bucket.get_object(research_base + file_name)
        return file_obj.read()
    return ''
    
def get_file(file_name):
    '''return tuning json string    
    '''
    global BUILD_DIR    
    str = download_from_my_oss(file_name)
    if(str):
        return str
        
    file_path = os.path.join(BUILD_DIR, file_name)        
    with open(file_path, 'r') as f:
        str = f.read()
    return str
    
def set_file(file_name, str):
    global BUILD_DIR
    file_path = os.path.join(BUILD_DIR, file_name)
    open(file_path, 'w').write(str)
    upload_to_my_oss(str, file_name)
        
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
            upload_to_my_oss(json_str, TUNING_FILE)
            f.write(json_str)
        print('tuning files written to %s' % tuning_file_path)
            
        