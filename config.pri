macx:zmq_dir = /usr/local/Cellar/zeromq/4.1.2
unix:!macx:zmq_dir = /usr/local

unix {
    zmq_include = $$zmq_dir/include
    zmq_lib = $$zmq_dir/lib
}

win32 {
    zmq_include = C:\zmq\include
    zmq_lib = C:\zmq\bin
}
