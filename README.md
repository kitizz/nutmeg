Nutmeg
======

Nutmeg is multiplatform, multilingual plotting tool. Primarily, it aims to be light, fast, and responsive, and has been designed to be the go to tool for visualizing and playing with data in the experimental phase of a project.

Notable Features:
- Plotting can be asynchronous with user's program
    + Can visualize the updating of data as your program runs
    + Fast
- Responsive panning and zooming of data using mouse or multitouch trackpad
- Lay out figure axes and plots using a more intuitive declarative style (QML)
    + Makes figure templates easy
- Lay out GUI controls that update parameters, which update the data

<img src="https://raw.githubusercontent.com/kitizz/nutmeg/master/images/readme-screenshot.png" alt="Preview" width="60%">

Nutmeg in Action
----------------

The following QML file declares a Figure which contains two axes. It uses QML style bindings to ensure they take up a certain percentage of the figure space.

**figure.qml**
```qml
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
```

From Python, data can be plotted like so:

```python
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
```

Result:

<img src="https://cloud.githubusercontent.com/assets/1326372/7898085/e24b9fe6-06c2-11e5-8237-e1cf1247e070.png" alt="Example" width="60%"/>

The Architecture
----------------

The Nutmeg core is a stand alone executable, and other programs can draw figures by sending the appropriate data, wrapped up in JSON, via sockets. The socket interface is built on the wonderful [ZMQ](http://zeromq.org/) enabling Nutmeg to support multiple platforms and languages.

The end user could in fact plot using the interface as it is, but a language-side API can simplify the process greatly. Therefore typically a user should become familiar with two interfaces: the QML-style Figure layout, and a language specific Nutmeg library for sending the Figure layout and updating its data.


Building
--------

### Requirements
- Qt 5.3 or higher
- [ZMQ](http://zeromq.org/intro:get-the-software) 3.2.x (4.0.x should technically work, but is untested)

### Unix
Before building, ensure that in `nutmeg.pro` the INCLUDEPATH and QMAKE_LIBDIR point to the include path and
library path for [zmq](http://zeromq.org/intro:get-the-software). If zmq has been built with the default
configuration in your favourite flavour of Unix, the default `nutmeg.pro` values should be fine.

### Windows
Need to try that yet. Let me know!

Goals
-----
- The GUI must be fast and responsive
- The GUI must never get in the way of the user or make them feel claustrophobic
- View precise information about the displayed data (data points)
- Fast, smooth data updating
- Work on multiple platforms and multiple languages
    + Nutmeg core should even run on mobile OSes (Android and iOS)
    + Nutmeg clients should run on desktop (Mac, Linux, Windows)

Non-Goals
---------
- Replace Matplotlib, GNU Plot, Matlab's plotting
- Output to PDF, etc (*for now*... with enough popularity, this will probably change)
