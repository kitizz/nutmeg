.. Nutmeg documentation master file, created by
   sphinx-quickstart on Thu Jul 10 14:46:26 2014.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to Nutmeg's documentation!
==================================

Nutmeg is multiplatform, multilingual plotting tool. Primarily, it aims to be light, fast, and responsive, and has been designed to be the go to tool for visualizing and playing with data in the experimental phase of a project.

Notable Features:

* Plotting can be asynchronous with user's program
    * Can visualize the updating of data as your program runs
    * Fast
* Responsive panning and zooming of data using mouse or multitouch trackpad
* Lay out figure axes and plots using a more intuitive declarative style (QML)
    * Makes figure templates easy
* Lay out GUI controls that update parameters, which update the data

Core Concept
------------
With Nutmeg, figures are laid out using QML (see :ref:`nutmeg-action`). These can be used as templates, making it easy to reuse figure styles.

The actual plot data, titles, labels, and other things unique to the figure can be set from your code. This paradigm is more similar to those of Matlab and Matplotlib.


Contents
--------

.. toctree::
    :maxdepth: 2
    :glob:

    getting_started
    qml_figures
    qml_gui
    client_socket_api
    about


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

