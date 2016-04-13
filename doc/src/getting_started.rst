Getting Started
===============

Prebuilt Binaries
-----------------
Prebuilt binaries are not available yet, but will be soon.

Building
--------

Requirements
^^^^^^^^^^^^
- Qt 5.3 or higher
- [ZMQ](http://zeromq.org/intro:get-the-software) 3.2.x (4.0.x should technically work, but is untested)

Unix
^^^^
Before building, ensure that in `nutmeg.pro` the INCLUDEPATH and QMAKE_LIBDIR point to the include path and library path for [zmq](http://zeromq.org/intro:get-the-software). If zmq has been built with the default configuration in your favourite flavour of Unix (e.g. :code:`apt-get install libzmq3-dev`), the default `nutmeg.pro` values should be fine.

Then from the source folder run::

    qmake
    make

This should provide an executable which is the Nutmeg core.

Windows
^^^^^^^
Need to try that yet. Let me know!

.. _nutmeg-action:

Nutmeg in Action
----------------

The Nutmeg core should be started by running the executable. Nutmeg will continue to run from the system tray if the main window is closed. The main window will return by clicking on the Nutmeg icon in the system tray and selcting "Show/Hide".

When the core is first started it will attempt to bind to the port nearest the default (43686). It will inform which port it has successfully bound to via a notification. The port number can be viewed or changed in the Settings window, available by right clicking the system tray icon (or odc icon in Mac).

The following QML file declares a Figure which contains two axes. It uses QML style bindings to ensure they take up a certain percentage of the figure space.

**figure.qml**

.. code-block:: qml

    Figure {
        id: fig
        Layouts.Column {
            spacing: 0.05*fig.height
            Axis {
                handle: "axis1"
                Layout.preferredHeight: 0.65*fig.height
                LinePlot {
                    handle: "data"
                    line { color: "#7777FF"; width: 4 }
                }
            }
            Axis {
                handle: "axis2"
                Layout.preferredHeight: 0.3*fig.height
                LinePlot {
                    handle: "data"
                    line { color: "#FF7777"; width: 4 }
                }
            }
        }
    }

From Python, data can be plotted like so:

.. code-block:: python

    import Nutmeg
    from numpy import sin, cos, pi, r_

    # Assuming the core is on port 43686 (default)
    Nutmeg.init()

    x = r_[0:1:0.01]
    y1 = sin(10*pi*x)
    y2 = 10*pi*cos(10*pi*x)

    fig = Nutmeg.figure("myFigure", "myFigure.qml")

    fig.set("axis1.data", x=x, y=y1)
    fig.set("axis2.data", x=x, y=y2)
