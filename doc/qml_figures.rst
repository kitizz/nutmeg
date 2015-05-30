QML Figure API
==============

This section explains how to layout a Nutmeg figure using QML. Most of these properties can also be set dynamically from code, and for many properties unique to a figure such as titles, labels, etc., this is recommended.

For example, to set a title from python code:

.. code-block:: qml

    fig = Nutmeg.figure("myFigure", "figure.qml")
    fig.set('ax.title', "My Figure")

Figure
------
See :doc:`Figure <qml_figures_figure>`

Axes
----
* :doc:`2D Axis <qml_figures_axis2d>`
* 3D Axis

Plots
-----
* XY Plot
* Bar Plot
* Image Plot
* Surface Plot


