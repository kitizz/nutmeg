import zmq
import numpy as np

import os

import Parallel
import threading


def toQmlObject(value, toplevel=False):
    '''
    Unfortunately, we need to apply this little bit of a hack in order to get
    multidimensional arrays over to QML. QML flattens [[1,2],[4,5]] for some
    reason. Hopefully this will change...
    :param topLevel: Should be true if this is the first dimension of a list
    '''
    if isinstance(value, np.ndarray):
        # We specify ndarray, because 'tolist' should be faster that iterating
        if value.dtype != 'O' and len(value.shape) == 1:
            if toplevel:
                return value.tolist()
            else:
                return {'list': value.tolist()}
        else:
            return {'list': [toQmlObject(subValue) for subValue in value]}

    elif isinstance(value, list):
        if toplevel:
            return toQmlObject(np.array(value), True)
        else:
            return {'list': [toQmlObject(subValue) for subValue in value]}

    elif isinstance(value, dict):
        for p in value:
            value[p] = toQmlObject(value[p], True)
        return value

    else:
        return value


# Threaded decorator
def threaded(fn):
    def wrapper(*args, **kwargs):
        threading.Thread(target=fn, args=args, kwargs=kwargs).start()
    return wrapper


class QMLException(Exception):
    pass


class Nutmeg:

    def __init__(self, address="tcp://localhost", port=1234, updatePort=2468):
        self.address = address + ":" + str(port)
        self.updateAddress = address + ":" + str(updatePort)
        # Create the socket and connect to it.
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REQ)
        self.socket.connect(self.address)
        self.socketLock = threading.Lock()

        self.figures = {}

        self._waitForUpdates()

    @threaded
    def _waitForUpdates(self):
        self.updateSocket = self.context.socket(zmq.REQ)
        self.updateSocket.connect(self.updateAddress)

        while True:
            self.updateSocket.send(b"ready")

            msg = self.updateSocket.recv_json()
            command, figureHandle, parameter, value = msg

            if command == 'updateParam':
                fig = self.figures[figureHandle]
                fig.updateParameter(parameter, value)

    def setValues(self, handle, properties, param=""):
        properties = toQmlObject(properties)
        # print properties

        self.socketLock.acquire()

        # encoder.FLOAT_REPR = lambda o: format(o, '.2g')
        msg = ["sendData", handle, properties, param]
        # msg = simplejson.dumps(pretty_floats(msg))
        self.socket.send_json(msg)

        reply = self.socket.recv_json()
        self.socketLock.release()
        if reply[0] != 0:  # Error
            print(reply)

    def figure(self, handle, figureDef):
        # We're going by the interesting assumption that a file path cannot be
        # used to define a QML layout...
        qml = ""
        if os.path.exists(figureDef):
            with open(figureDef, 'r') as F:
                qml = F.read().encode('UTF-8')
        else:
            qml = figureDef

        self.socket.send_json(["createFigure", handle, qml])
        reply = self.socket.recv_json()
        if reply[0] != 0:  # Error
            err = reply[1]['qmlErrors'][0]
            msg = "At line %d, col %d: %s" % \
                (err['lineNumber'], err['columnNumber'], err['message'])
            raise(QMLException(msg))
            return None

        else:
            fig = Figure(self, reply[1])  # Should contain the figure's handle
            self.figures[reply[1]] = fig
            return fig


class Figure():
    def __init__(self, nutmeg, handle):
        self.nutmeg = nutmeg
        self.handle = handle
        self.parameters = {}
        self.updates = {}

    def setGui(self, guiDef):
        # We're going by the interesting assumption that a file path cannot be
        # used to define a QML layout...
        qml = ""
        if os.path.exists(guiDef):
            with open(guiDef, 'r') as F:
                qml = F.read().encode('UTF-8')
        else:
            qml = guiDef

        nutmeg = self.nutmeg
        # Note that this method of multithreading sockets is frowned upon in the PyZMQ docs
        nutmeg.socketLock.acquire()
        nutmeg.socket.send_json(["createGui", self.handle, qml])
        # print("JSON sent...")
        reply = nutmeg.socket.recv_json()
        nutmeg.socketLock.release()
        if reply[0] != 0:  # Error
            err = reply[1]['qmlErrors'][0]
            msg = "At line %d, col %d: %s" % \
                (err['lineNumber'], err['columnNumber'], err['message'])
            raise(QMLException(msg))

        else:
            # Reply should contain a mapping of the parameters and their values
            # Init any updates that suit the new params.
            self.parameters = reply[2]
            updatesToCall = set()
            for p in self.updates:
                if p in self.parameters:
                    updatesToCall = updatesToCall.union(self.updates[p])

            # We use a set so updates aren't initialised multiple times
            for update in updatesToCall:
                update.initializeParameters(self.parameters)

            return True

    def updateParameter(self, param, value):
        # Mmm..... boilerplate...
        self.parameters[param] = value
        updatesToCall = self.updates[param]

        # We use a set so updates aren't initialised multiple times
        for update in updatesToCall:
            update.parametersChanged(self.parameters)

    def set(self, handle, properties, param=""):
        '''
        Set the properties in `handle` according to the provided dictionary of
        properties and their values.

        For example: figure.set('ax[1].data', {'x': range(10), 'y': someData})
        
        :param handle: A string to the object of interest
        :param properties: A dictionary mapping properties to values
        '''
        fullHandle = self.handle + "." + handle

        if isinstance(properties, Updater):
            self.registerUpdate(properties, handle)
        else:
            return self.nutmeg.setValues(fullHandle, properties, param)

    def getParameterValues(self):
        '''
        :return: A dictionary of the parameters and their values.
        '''
        # TODO
        pass

    def registerUpdate(self, update, handle):
        # The update needs to know the figure and handle of the targets
        update.handle = handle
        update.figure = self
        # Add update to the list for each registered parameter
        requiredParamsLoaded = True
        for param in update.params:
            if param not in self.updates:
                self.updates[param] = []
            self.updates[param].append(update)
            # Check if this parameter is loaded
            if requiredParamsLoaded and param not in self.parameters:
                requiredParamsLoaded = False

        if requiredParamsLoaded:
            update.initializeParameters(self.parameters)


class Updater():
    def __init__(self, params, init=None, update=None):
        '''
        :param params: A list of parameter names that this update depends on.
        '''
        self.params = params
        self.init = init
        self.update = update
        self.initialized = False
        # Figure and handle should be defined when the update is registered
        self.figure = None
        self.handle = ""

        self.eventFunc = None
        self.eventParams = {}
        self.eventParam = ""  # Which parameter caused this event
        self.updateEvent = threading.Event()
        self._processAndSend()

    @threaded
    def _processAndSend(self):
        ''' Wait until an update needs to be called. Call it in a separate
        process (because GIL), send back the result, and wait for another. '''
        while True:
            # When the event triggers, let's roll.
            # print("Update waiting %s" % self.params)
            self.updateEvent.wait()
            # Clear the event func and parameters to clean things up
            func, params, param = self.eventFunc, self.eventParams, self.eventParam
            self.eventFunc, self.eventParams, self.eventParam = None, {}, ""
            self.updateEvent.clear()

            # print("Received event")
            if not self.figure or not self.handle:
                continue  # What's the point...
            # print("Processing in parallel")
            # Start the update in a separate process.
            result = Parallel.sub_process(func, params)
            # print ("Received result...")
            self.figure.set(self.handle, result, param)

    def initializeParameters(self, params):
        # print("Initialise parameters for %s" % params)
        self.initialized = True
        func = None
        if self.init is not None:
            func = self.init
        elif self.update is not None:
            func = self.update
        else:
            return
        if not self.updateEvent.isSet():
            self.eventFunc = func
            self.eventParams = params
            self.updateEvent.set()

    def parametersChanged(self, params, param=""):
        '''
        :param params: Dictionary of parameters and their values.
        :param param: The parameter which triggered this event.
        '''
        # print("Parameters changed for %s" % params)
        if self.update is None: return
        if not self.updateEvent.isSet():
            self.eventFunc = self.update
            self.eventParams = params
            self.eventParam = param
            self.updateEvent.set()


from scipy import ndimage
import time
def applyBlur(dataIn, sigma):
    return ndimage.gaussian_filter1d(dataIn, sigma, axis=1)


def _testParams():
    nm = Nutmeg()
    fig = nm.figure('fig', 'figure1.qml')

    print "Sending GUI..."
    success = fig.setGui('gui1.qml')

    N = 100
    data = np.random.standard_normal((3,N*10))
    data2 = np.random.standard_normal((3,N))

    update = lambda params: { 'y': applyBlur(data, params['sigma']) }

    update2 = lambda params: { 'y': applyBlur(data2, params['sigma']) }

    fig.set('ax[:].blue', {'x': np.arange(N*10,dtype=float)/10})
    fig.set('ax[:].red', {'x': np.arange(N, dtype=float)})
    fig.set('ax[:].blue', Updater(['sigma'], update=update))
    fig.set('ax[:].red', Updater(['sigma'], update=update2))
    print("Data set")


def _testBasic():
    nm = Nutmeg()
    fig = nm.figure('fig', 'figure1.qml')
    nm.set(fig+'.ax[1].data', {'x': range(10), 'y': np.random.standard_normal(10)})


if __name__ == '__main__':
    _testParams()
    # _testFloats()
    # print toQmlObject({'xdata': {0:[1,2,3], 1:[[4],5,6], 2:np.eye(3)}, 'ydata': range(4)})
