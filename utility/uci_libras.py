"""Libras Identification dataset.
The original dataset description was available from 
    https://archive.ics.uci.edu/ml/datasets/Libras+Movement

"""

# License: BSD 3 clause

from os.path import dirname, exists, join
from os import makedirs, remove

import numpy as np

from sklearn.datasets.base import get_data_home
from sklearn.datasets.base import _fetch_remote
from sklearn.datasets.base import RemoteFileMetadata
from sklearn.datasets.base import _pkl_filepath
from sklearn.utils import _joblib
from sklearn.utils import check_random_state, Bunch

LIBRAS = RemoteFileMetadata(
    filename='libras.data',
    url='https://archive.ics.uci.edu/ml/machine-learning-databases/libras/movement_libras.data',
    checksum=('97ebdaa6a9b28ab4a2cdd84b14f19a95'
              'a7456a46137c362b65a0669eca3c3c4d'))


def fetch_uci_libras(data_home=None, shuffle=False, random_state=0,
                         download_if_missing=True):
    """Load the UCI libra  data-set from AT&T (classification).
    Download it if necessary.

    =================   =====================
    Classes                                15
    Samples total                         360
    Dimensionality                       90
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
    data : numpy array of shape (360, 90)
        Each row corresponds to a libras feature of 9 dimension
    target : numpy array of shape (360, )
        Labels associated to each glas. Those labels are from
        range(15) and correspond to the Subject IDs.
    """
    global LIBRAS
    data_home = get_data_home(data_home=data_home)
    if not exists(data_home):
        makedirs(data_home)
    filepath = _pkl_filepath(data_home, 'uci_libras.pkz')
    if not exists(filepath):
        if not download_if_missing:
            raise IOError("Data not found and `download_if_missing` is False")

        print('downloading UCI LIBRAS from %s to %s'
              % (LIBRAS.url, data_home))
        data_path = _fetch_remote(LIBRAS, dirname=data_home)

        libras = np.genfromtxt(data_path, delimiter=",")
        _joblib.dump(libras, filepath, compress=6)
        remove(data_path)
        
    else:
        libras = _joblib.load(filepath)

    feature = libras[:,0:-1]
    target = libras[:,-1]
    if shuffle:
        random_state = check_random_state(random_state)
        order = random_state.permutation(len(libras))
        feature = libras[order]
        target = target[order]


    return (feature,target)
if __name__ == '__main__':
    fetch_uci_libras()