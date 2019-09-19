.. principal sequence of partition documentation master file, created by
   sphinx-quickstart on Thu Sep  5 18:27:13 2019.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to principal sequence of partition's documentation!
===========================================================
Introduction
------------
Principal sequence of partition is a method to obtain a successive finer partitions for an undirect
or directed graph. It has applications in network reliability, community detection and Potts model in Physics.
The first non-trival partition is associated with 
the `graph strength <https://en.wikipedia.org/wiki/Strength_of_a_graph>`_. The following two paragraphs give a very short Introduction
of what is principal sequence of partition.

Let us consider an undirected graph with weights :math:`c_e`, for a given cost :math:`\lambda\geq 0`, we 
would like to find a partition :math:`\mathcal{P}` such that the following objective function is minimized:

.. math::
   f(\mathcal{P}) = c(\delta(\mathcal{P})) - \lambda |\mathcal{P}|

:math:`\delta(\mathcal{P})` is the collection of edges whose endpoints belong to different components of the
partition.

For increasing value of :math:`\lambda`, we can get a nested partitions :math:`\mathcal{P}_1, \dots, \mathcal{P}_k`.
Every successive pair in this partition sequence corresponds to some unique value of :math:`\lambda`. The
resulting increasing sequence of :math:`\lambda` is called the sequence of critical values.

This repository contains source code to compute the principal sequence of partition for (un)directed graph.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   install
   usage
   detail
   api
   
References
----------
.. bibliography:: refs.bib
   :all:

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
