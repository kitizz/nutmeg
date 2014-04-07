
import zmq
import numpy as np
import time
import timeit

context = zmq.Context()

#  Socket to talk to server
print("Connecting to hello world server...")
socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:1234")

qml = ""
with open('figure1.qml', 'r') as F:
    qml = F.read().encode('UTF-8')

#  Create a new figure according to qml
print("Sending request for new figure...")
socket.send_json(["createFigure", "fig2", qml])
print("Received reply: %s" % socket.recv_json())


print("Sending data")

N = 10000
x = [i for i in range(N)]
msg = ["sendData", "fig2.ax.data", {'x':x}]
socket.send_json(msg)
print("Received reply: %s" % socket.recv_json())

def fireAway():
    y = list(np.random.standard_normal(N))
    msg = ["sendData", "fig2.ax.data", {'y':y}]
    socket.send_json(msg)
    print("Received reply: %s" % socket.recv_json())

tests = 100
res = timeit.timeit('fireAway()', 'from __main__ import fireAway', number=tests)

print("FPS of update: %f" % (tests/res))
