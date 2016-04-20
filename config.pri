macx:zmq_dir = /usr/local/Cellar/zeromq/4.1.2
unix:!macx:zmq_dir = /usr/local
win32:zmq_dir = C:\libzmq

unix {
    zmq_include = $$zmq_dir/include
    zmq_lib = $$zmq_dir/lib
}

win32 {
    zmq_include = $$zmq_dir\include
    zmq_lib = $$zmq_dir\bin\x64\Debug\v140\dynamic
}
