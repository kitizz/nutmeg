import zmq
import numpy as np

import os
import sys

import Parallel
import threading

import signal


_nutmegCore = None
_original_sigint = None


def init(address="tcp://localhost", port=43686, timeout=3000):
    global _nutmegCore

    if _nutmegCore is not None:
        print("Nutmeg client already initialized.")
        return
    _nutmegCore = Nutmeg(address, port, timeout)


def initialized():
    return _nutmegCore is not None


def figure(handle, figureDef):
    if _nutmegCore is None:
        raise(NutmegException("The Nutmeg client needs to be initialized (Nutmeg.init(...)."))
    return _nutmegCore.figure(handle, figureDef)


def toQmlObject(value):
    '''
    Recursively convert any numpy.ndarrays into lists in preparation for
    JSONification.
    '''
    if isinstance(value, np.ndarray):
        # We specify ndarray, because 'tolist' should be faster that iterating
        if value.dtype != 'O':
            return value.tolist()
        else:
            return [toQmlObject(subValue) for subValue in value]

    elif isinstance(value, list):
        return [toQmlObject(subValue) for subValue in value]

    elif isinstance(value, dict):
        for p in value:
            value[p] = toQmlObject(value[p])
        return value

    else:
        return value


textchars = ''.join(map(chr, [7,8,9,10,12,13,27] + range(0x20, 0x100)))
def isBinary(data):
    return bool(data.translate(None, textchars))


class QMLException(Exception):
    pass


class NutmegException(Exception):
    pass


# Threaded decorator
def _threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.qudaemon = True
        thread.start()
    return wrapper


class Nutmeg:

    def __init__(self, address="tcp://localhost", port=43686, timeout=3000):
        '''
        :param timeout: Timeout in ms
        '''
        self.host = address
        self.port = port
        self.address = address + ":" + str(port)

        # Create the socket and connect to it.
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REQ)
        self.socket.setsockopt(zmq.LINGER, 0)
        self.socket.connect(self.address)

        # Set up a poller for timeouts
        self.poller = zmq.Poller()
        self.poller.register(self.socket, zmq.POLLIN)
        self.timeout = timeout

        self.socketLock = threading.Lock()

        self.figures = {}

        # Overwrite the signal interupt
        global _original_sigint
        _original_sigint = signal.getsignal(signal.SIGINT)
        signal.signal(signal.SIGINT, exit_gracefully)

    def setValues(self, handle, properties, param=""):
        properties = toQmlObject(properties)

        if not isinstance(properties, dict):
            handleExpanded = handle.split('.')
            properties = {handleExpanded[-1]: properties}
            handle = '.'.join( handleExpanded[:-1] )

        binaryProps = {}
        for key in properties:
            if isBinary(properties[key]):
                binaryProps[key] = properties[key]
        # Pop them out afterwards
        for key in binaryProps:
            properties.pop(key)

        if len(properties) > 0:
            msg = {"handle": handle, "data": properties, "parameter": param }
            self.socketLock.acquire()
            self.send_json(["sendData", msg])
            reply = self.recv_json()
            self.socketLock.release()
            # Error
            if reply[0] != 0:
                raise(NutmegException(reply[1]['message']))

        if len(binaryProps) > 0:
            for key in binaryProps:
                msg = {"handle": handle, "property": key, "parameter": param }
                self.socketLock.acquire()
                self.send_binary(msg, binaryProps[key])
                reply = self.recv_json()
                self.socketLock.release()
                # Error
                if reply[0] != 0:
                    raise(NutmegException(reply[1]['message']))

        

    def figure(self, handle, figureDef):
        # We're going by the interesting assumption that a file path cannot be
        # used to define a QML layout...
        qml = ""

        if figureDef.endswith('.qml') or os.path.exists(figureDef):
            if os.path.exists(figureDef):
                with open(figureDef, 'r') as F:
                    qml = F.read().encode('UTF-8')
            else:
                raise(QMLException("File, %s, does not exist." % figureDef))
        else:
            qml = figureDef

        self.socketLock.acquire()
        self.send_json(["createFigure", {"figureHandle": handle, "qml": qml}])
        reply = self.recv_json()
        self.socketLock.release()

        if reply[0] != 0:  # Error
            err = reply[1]
            msg = "At line %d, col %d: %s" % \
                (err['lineNumber'], err['columnNumber'], err['message'])
            raise(QMLException(msg))
            return None

        else:
            msg = reply[1]
            h = msg["figureHandle"]
            fig = Figure(self, handle=h,
                         address=self.host, port=msg["port"])
            self.figures[h] = fig
            return fig

    def send_json(self, msg):
        self.socket.send_string("json", flags=zmq.SNDMORE)
        self.socket.send_json(msg)

    def send_binary(self, msg, data):
        self.socket.send_string("binary", flags=zmq.SNDMORE)
        self.socket.send_json(msg, flags=zmq.SNDMORE)
        self.socket.send(data)

    def recv_json(self, timeout=None):
        if timeout is None:
            timeout = self.timeout

        if self.poller.poll(self.timeout):
            return self.socket.recv_json()
        else:
            raise IOError("Timeout receiving response from Nutmeg. Check that %s and \
%d are the correct host and port." % (self.host, self.port))


class Figure():
    def __init__(self, nutmeg, handle, address, port):
        self.nutmeg = nutmeg
        self.handle = handle
        self.parameters = {}
        self.updates = {}
        self.updateAddress = address + ":" + str(port)

        self._waitForUpdates()

    @_threaded
    def _waitForUpdates(self):
        self.updateSocket = self.nutmeg.context.socket(zmq.REQ)
        print("Connecting to socket at: %s" % self.updateAddress)
        self.updateSocket.connect(self.updateAddress)
        print("Connected")

        while True:
            self.updateSocket.send(b"ready")

            msg = self.updateSocket.recv_json()
            command = msg[0]

            if command == 'updateParam':
                figureHandle, parameter, value = msg[1:4]
                self.updateParameter(parameter, value)
            elif command == 'ping':
                pass

    def setGui(self, guiDef):
        # We're going by the interesting assumption that a file path cannot be
        # used to define a QML layout...
        qml = ""
        if os.path.exists(guiDef):
            with open(guiDef, 'r') as F:
                qml = F.read().encode('UTF-8')
        else:
            qml = guiDef

        nutmeg = self.nutmeg  # Helps code readability

        # Note that this method of multithreading sockets is frowned upon in the PyZMQ docs
        nutmeg.socketLock.acquire()
        nutmeg.send_json(["createGui", {"figureHandle": self.handle, "qml": qml}])
        # print("JSON sent...")
        reply = nutmeg.recv_json()
        nutmeg.socketLock.release()

        if reply[0] != 0:  # Error
            err = reply[1]
            msg = "At line %d, col %d: %s" % \
                (err['lineNumber'], err['columnNumber'], err['message'])
            raise(QMLException(msg))

        else:
            # Reply should contain a mapping of the parameters and their values
            # Init any updates that suit the new params.
            msg = reply[1]
            self.parameters = msg["parameters"]
            updatesToCall = set()
            for p in self.updates:
                if p in self.parameters:
                    updatesToCall = updatesToCall.union(self.updates[p])

            # We use a set so updates aren't initialised multiple times
            for update in updatesToCall:
                update.initializeParameters(self.parameters)

            # Now that we have a GUI, we need to wait for updates.
            if len(self.parameters) > 0:
                self._waitForUpdates()

            return True

    def updateParameter(self, param, value):
        # Mmm..... boilerplate...
        self.parameters[param] = value
        updatesToCall = self.updates[param]

        # We use a set so updates aren't initialised multiple times
        for update in updatesToCall:
            update.parametersChanged(self.parameters, param)

    def set(self, handle, properties, param=""):
        '''
        Set the properties in `handle` according to the provided dictionary of
        properties and their values.

        For example: figure.set('ax[1].data', {'x': range(10), 'y': someData})
        
        :param handle: A string to the object or property of interest
        :param properties: A dictionary mapping properties to values or just the property values themselves. If `properties` is not a dictionary, Nutmeg will assume that `handle` points directly to a property.
        '''
        fullHandle = self.handle + "." + handle

        if isinstance(properties, Updater):
            self.registerUpdate(properties, handle)
        else:
            try:
                return self.nutmeg.setValues(fullHandle, properties, param)
            except IOError as e:
                print("IOError in Nutmeg core...")
                print(e)

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

    @_threaded
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
            # print("Received result...")
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


def exit_gracefully(signum, frame):
    # stackoverflow.com/a/18115530/1512137
    # restore the original signal handler as otherwise evil things will happen
    # in raw_input when CTRL+C is pressed, and our signal handler is not re-entrant
    signal.signal(signal.SIGINT, _original_sigint)

    try:
        if raw_input("\nReally quit? (y/n)> ").lower().startswith('y'):
            sys.exit(1)

    except KeyboardInterrupt:
        print("Ok ok, quitting")
        sys.exit(1)

    # restore the exit gracefully handler here    
    signal.signal(signal.SIGINT, exit_gracefully)
