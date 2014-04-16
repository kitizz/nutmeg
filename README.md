Nutmeg
======

Building
------

### Requirements
- Qt 5.3 or higher
- [ZMQ](http://zeromq.org/intro:get-the-software) 3.2.x (4.0.x should technically work, but is untested)

### Unix
Before building, ensure that in `nutmeg.pro` the INCLUDEPATH and QMAKE_LIBDIR point to the include path and
library path for [zmq](http://zeromq.org/intro:get-the-software). If zmq has been built with the default
configuration in your favourite flavour of Unix, the default `nutmeg.pro` values should be fine.

### Windows
Need to try that yet. Let me know!
