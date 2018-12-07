"""Glass Identification dataset.
The original dataset description was available from 
    https://archive.ics.uci.edu/ml/datasets/glass+identification

"""

# License: BSD 3 clause

from os.path import dirname, exists, join
from os import makedirs, remove

import numpy as np
from scipy.io.matlab import loadmat

from sklearn.datasets.base import get_data_home
from sklearn.datasets.base import _fetch_remote
from sklearn.datasets.base import RemoteFileMetadata
from sklearn.datasets.base import _pkl_filepath
from sklearn.utils import _joblib
from sklearn.utils import check_random_state, Bunch

# The original data can be found at:
# https://cs.nyu.edu/~roweis/data/olivettiGLASS.mat
GLASS = RemoteFileMetadata(
    filename='glass.data',
    url='https://archive.ics.uci.edu/ml/machine-learning-databases/glass/glass.data',
    checksum=('dd67373f4baf2807345df02cbfef2093'
              'd342e61ad0d82a4fb79af43ef8ce449d'))


def fetch_uci_glass(data_home=None, shuffle=False, random_state=0,
                         download_if_missing=True):
    """Load the UCI glass data-set from AT&T (classification).
    Download it if necessary.
    =================   =====================
    Classes                                6
    Samples total                         214
    Dimensionality                       9
    Features                            real
    =================   =====================
    
    Parameters
    ----------
    data_home : optional, default: None
        Specify another download and cache folder for the datasets. By default
        all scikit-learn data is stored in '~/scikit_learn_data' subfolders.
    shuffle : boolean, optional
        If True the order of the dataset is shuffled to avoid having
        images of the same person grouped.
    random_state : int, RandomState instance or None (default=0)
        Determines random number generation for dataset shuffling. Pass an int
        for reproducible output across multiple function calls.
        See :term:`Glossary <random_state>`.
    download_if_missing : optional, True by default
        If False, raise a IOError if the data is not locally available
        instead of trying to download the data from the source site.
    Returns
    -------
    An object with the following attributes:
    data : numpy array of shape (214, 9)
        Each row corresponds to a glass feature of 9 dimension
    target : numpy array of shape (214, )
        Labels associated to each glas. Those labels are from
        [1,2,3,5,6,7] and correspond to the Subject IDs.
    DESCR : string
        Description of the UCI Glass Dataset.
    """
    global GLASS
    data_home = get_data_home(data_home=data_home)
    if not exists(data_home):
        makedirs(data_home)
    filepath = _pkl_filepath(data_home, 'uci_glass.pkz')
    if not exists(filepath):
        if not download_if_missing:
            raise IOError("Data not found and `download_if_missing` is False")

        print('downloading UCI GLASS from %s to %s'
              % (GLASS.url, data_home))
        data_path = _fetch_remote(GLASS, dirname=data_home)

        glass = np.genfromtxt(data_path, delimiter=",")
        _joblib.dump(glass, filepath, compress=6)
        remove(data_path)
        
    else:
        glass = _joblib.load(filepath)

    feature = glass[:,1:-1]
    target = glass[:,-1]
    if shuffle:
        random_state = check_random_state(random_state)
        order = random_state.permutation(len(glass))
        feature = glass[order]
        target = target[order]


    return Bunch(data=feature,
                 target=target,
                 DESCR='uci_glass_dataset')
if __name__ == '__main__':
    fetch_uci_glass()