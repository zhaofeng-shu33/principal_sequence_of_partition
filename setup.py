# we use this setup.py to build wheels of info-clustering package
# this kind of installation is more flexible and maintainable than `cmake install`
# you can only choose one of the two installation methods.
# before running this file, make sure psp dynamic lib exists in build directory
from setuptools import setup
setup(
    name='info-clustering',
    version='0.1',
    packages=['info_cluster'],
    install_requires=['numpy'],
    author="zhaofeng-shu33",
    author_email="616545598@qq.com",
    description="a hierachical clustering algorithm based on information theory",
    url="https://github.com/zhaofeng-shu33/principal_sequence_of_partition",
    classifiers=[
        "Programming Language :: Python :: 3",
    ],    
)