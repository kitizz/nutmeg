from __future__ import print_function, division
import zmq
import numpy as np

import os
import sys

import ParallelNutmeg as Parallel
import threading
import time

import signal


_nutmegCore = None
_original_sigint = None
_address = "tcp://localhost"
_port = 43686
_timeout = 500

# _defaults = dict(address="tcp://localhost", port=43686, timeout=100)


def init(address=_address, port=_port, timeout=_timeout):
    # global _nutmegCore

    # if _nutmegCore is not None:
    #     if _nutmegCore.initialized:
    #         print("Nutmeg client already initialized.")
    #     return
    # _nutmegCore = Nutmeg(address, port, timeout)
    _core(address, port, timeout)


def _core(address=_address, port=_port, timeout=_timeout):
    global _nutmegCore

    if _nutmegCore is None:
        _nutmegCore = Nutmeg(address, port, timeout)

    elif address != _address or \
            port != _port or \
            timeout != _timeout:
        print("WARNING: Module's Nutmeg core already exists with different settings. For multiple instances, manually instantiate a Nutmeg object.")

    return _nutmegCore


def initialized():
    return _nutmegCore is not None and _nutmegCore.initialized


def figure(handle, figureDef, assync=True):
    return _core().figure(handle, figureDef, assync)


def isValidHandle(handle):
    return _core().isValidHandle(handle)


def toQmlObject(value, binary=False):
    '''
    Recursively convert any numpy.ndarrays into lists in preparation for
    JSONification.
    '''
    if isinstance(value, np.ndarray):
        # We specify ndarray, because 'tolist' should be faster that iterating
        # if np.isnan(value).any():
        #     # value[np.isnan(value)] = 0
        #     raise Exception("NaNs in array!")

        if binary:
            return value
        elif value.dtype != 'O':  # Check not object type
            return value.tolist()
        else:
            return [toQmlObject(subValue) for subValue in value]

    elif isinstance(value, list):
        return [toQmlObject(subValue) for subValue in value]

    elif isinstance(value, dict):
        for p in value:
            # TODO: DEFINITELY remove this binary hack once general binary-izing of arrays is implemented.
            value[p] = toQmlObject(value[p], binary=(p == 'binary'))
        return value

    else:
        return value


# textchars = bytearray([7,8,9,10,12,13,27] + list(range(0x20, 0x100)))
# def isBinary(data):
#     if type(data) is not bytes:
#         return False
#     return bool(data.translate(None, textchars))


class QMLException(Exception):
    pass


class NutmegException(Exception):
    pass


# Threaded decorator
def _threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.daemon = True
        thread.start()
    return wrapper


class Nutmeg:

    def __init__(self, address=_address, port=_port, timeout=_timeout, pingperiod=10000):
        '''
        :param timeout: Timeout in ms
        '''
        self.initialized = False
        self.host = address
        self.port = port
        self.address = address + ":" + str(port)
        self.timeout = timeout

        # Create the socket and connect to it.
        self.context = zmq.Context()
        self.socket = None
        self.poller = None
        self.reset_socket()

        self.socketLock = threading.Lock()

        self.figures = {}

        self.pingperiod = pingperiod/1000
        self.initialized = self.ping()

    def reset_socket(self):
        # Close things if necessary
        if self.socket is not None:
            if self.poller is not None:
                self.poller.unregister(self.socket)
            self.socket.close()

        self.socket = self.context.socket(zmq.REQ)
        self.socket.setsockopt(zmq.LINGER, 0)
        self.socket.connect(self.address)

        self.poller = zmq.Poller()
        self.poller.register(self.socket, zmq.POLLIN)

        # Last time a disconnection occurred
        self.disconnected_t = time.time()

    def ready(self):
        # Don't ping everytime so as not to slow down a program too much if Nutmeg isn't around
        # But allow constant attempts up to 2seconds after the disconnection
        if not self.initialized and \
                (time.time() - self.last_ping > self.pingperiod or time.time() - self.disconnected_t < 2):
            self.initialized = self.ping()
        return self.initialized

    # @_threaded
    def set_values(self, handle, *value, **properties):
        '''
        Set properties of handle. It will be assumed that handle is a property
        itself if *value contains any values.

        :param handle: Handle to an object or property.
        :param *value: If handle refers to a property directly, the first
            element of this list will be used.
        :param **properties: Keyword args defining the property names and their
            values. Will only be used if *value is empty.
        :param param: Indicate if these new values are in response to a change
            in parameter, param.
        :param binary: Indicate that the provided data is to be sent as binary
        '''
        if 'param' in properties:
            param = properties.pop('param')
        else:
            param = ""

        if len(value) > 0:
            handleExpanded = handle.split('.')
            properties = {handleExpanded[-1]: value[0]}
            handle = '.'.join( handleExpanded[:-1] )

        else:
            properties = toQmlObject(properties)

        # Grab the binary property if it exists
        binaryProps = {}
        if 'binary' in properties:
            binaryProps['binary'] = properties.pop('binary')

        if len(properties) > 0:
            msg = {"handle": handle, "data": properties, "parameter": param }
            reply = self.send_recv_json(["sendData", msg])
            # Error
            if reply[0] != 0:
                raise(NutmegException(reply[1]['message']))

        if len(binaryProps) > 0:
            # NB: Kept like this for future generalizations...
            # TODO: For images, consider this in the future: https://code.google.com/p/lz4/
            for key in binaryProps:
                value = binaryProps[key]
                if type(value) == np.ndarray:
                    shape = list(value.shape)
                elif type(value) == bytes:
                    shape = [len(value)]
                else:
                    print(type(value))
                    raise NutmegException("Error converting %s.%s to binary value. Must be type np.ndarray or bytes" % (handle, key))

                msg = {"handle": handle, "property": key, "parameter": param,  "shape": shape}
                reply = self.send_recv_binary(msg, value)
                # Error
                if reply[0] != 0:
                    raise(NutmegException(reply[1]['message']))

    def figure(self, handle, figureDef, assync=True):
        # We're going by the interesting assumption that a file path cannot be
        # used to define a QML layout...
        qml = ""

        if figureDef.endswith('.qml') or os.path.exists(figureDef):
            if os.path.exists(figureDef):
                with open(figureDef, 'r') as F:
                    qml = F.read()  #.encode('UTF-8')
            else:
                raise(QMLException("File, %s, does not exist." % figureDef))
        else:
            qml = figureDef

        return Figure(_core(), handle, address=self.host, port=self.port, qml=qml, assync=assync)

    def isValidHandle(self, handle):
        self.socketLock.acquire()
        self.send_json(["handleValid", {"handle": handle}])
        reply = self.recv_json()
        self.socketLock.release()

        # Reply is 0 if valid, 6 if invalid.
        return reply[0] == 0

    def send_json(self, msg):
        self.socket.send_string("json", flags=zmq.SNDMORE)
        self.socket.send_json(msg)

    def send_binary(self, msg, data):
        self.socket.send_string("binary", flags=zmq.SNDMORE)
        self.socket.send_json(msg, flags=zmq.SNDMORE)
        self.socket.send(data, copy=False)

    def recv_json(self, timeout=None):
        if timeout is None:
            timeout = self.timeout

        if self.poller.poll(timeout):
            return self.socket.recv_json()
        else:
            self.reset_socket()
            self.initialized = False
            self.disconnected_t = time.time()
            raise IOError("Timeout receiving response from Nutmeg. Check that %s and \
%d are the correct host and port." % (self.host, self.port))

    def send_recv_json(self, msg, timeout=None):
        self.socketLock.acquire()
        # print(msg)
        self.send_json(msg)
        try:
            reply = self.recv_json(timeout)
        except IOError:
            # self.socketLock.release()
            # self.initialized = False
            raise
        finally:
            self.socketLock.release()

        return reply

    def send_recv_binary(self, msg, data, timeout=None):
        self.socketLock.acquire()
        self.send_binary(msg, data)
        try:
            reply = self.recv_json(timeout)
        except IOError:
            # self.socketLock.release()
            # self.initialized = False
            raise
        finally:
            self.socketLock.release()

        return reply

    def ping(self, timeout=None):
        if timeout is None:
            timeout = self.timeout

        msg = ["ping", {}]
        self.socketLock.acquire()
        self.last_ping = time.time()
        self.send_json(msg)
        try:
            self.recv_json(timeout)
            result = True
        except IOError:
            result = False
        finally:
            self.socketLock.release()

        return result


class NutmegObject(object):
    def __init__(self, handle):
        self.handle = handle

    def __getattr__(self, name):
        newHandle = self.handle + '.' + name
        if isValidHandle(newHandle):
            return NutmegObject(newHandle)
        else:
            raise(NutmegException("%s has no object with handle, %s" % (self.handle, name)))


class Figure(NutmegObject):
    def __init__(self, nutmeg, handle, address, port, qml, assync=True):
        self.nutmeg = nutmeg
        self.handle = handle
        self.qml = qml
        self.parameters = {}
        self.updates = {}
        self.address = address
        self.updateAddress = address + ":" + str(port)

        self.assync = assync

        self._setlock = threading.Lock()

        self.sent = False
        self.send()

    def send(self):
        if self.sent:
            return

        if self.nutmeg.ready():
            figdef = dict(figureHandle=self.handle, qml=self.qml)
            # try:
            reply = self.nutmeg.send_recv_json(["createFigure", figdef])
            # except IOError as e:
                # print(e)

        if not self.nutmeg.initialized:
            print("WARNING: Figure not sent (Nutmeg figure.nutmeg instance not yet initialized).")
            return

        if reply[0] != 0:  # Error
            err = reply[1]
            row, col = err['lineNumber'], err['columnNumber']
            msg = "At line %d, col %d: %s" % \
                (row, col, err['message'])
            raise(QMLException(msg))
            # return Figure(None, handle, address=self.host, port=self.port)

        else:
            msg = reply[1]
            self.handle = msg['figureHandle']
            self.port = msg['port']
            self.nutmeg.figures[self.handle] = self
            self.sent = True
            self.updateAddress = self.address + ":" + str(self.port)
            # return fig

    @_threaded
    def _waitForUpdates(self):
        self.updateSocket = self.nutmeg.context.socket(zmq.REQ)
        print("Connecting to socket at: %s" % self.updateAddress)
        self.updateSocket.connect(self.updateAddress)
        print("Connected")

        while True:
            # print("Send socket ready")
            self.updateSocket.send(b"ready")

            msg = self.updateSocket.recv_json()
            command = msg[0]
            # print("Received update %s" % msg)

            if command == 'updateParam':
                figureHandle, parameter, value = msg[1:4]
                self.updateParameter(parameter, value)
            elif command == 'ping':
                time.sleep(0.01)

    def setGui(self, guiDef):
        # We're going by the interesting assumption that a file path cannot be
        # used to define a QML layout...
        qml = ""
        if os.path.exists(guiDef):
            with open(guiDef, 'r') as F:
                qml = F.read()  #.encode('UTF-8')
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
            for p in msg["parameters"]:
                self.parameters[p] = Parameter(p, msg["parameters"][p], False)
            updatesToCall = set()
            # Just in case updates have already been registered, add them to the
            # list for initialisation.
            for p in self.updates:
                if p in msg["parameters"]:
                    updatesToCall = updatesToCall.union(self.updates[p])

            # We use a set so updates aren't initialised multiple times
            for update in updatesToCall:
                update.initializeParameters(self.parameters)

            # Now that we have a GUI, we need to wait for updates.
            if len(self.parameters) > 0:
                self._waitForUpdates()

            return True

    def updateParameter(self, param, value):
        # print("Update Parameter:", param, value)
        self.parameters[param].updateValue(value)

        # Call updates if there are any attached to this parameter
        if param not in self.updates:
            return
        updatesToCall = self.updates[param]

        # We use a set so updates aren't initialised multiple times
        for update in updatesToCall:
            update.parametersChanged(self.parameters, param)

    def parameter(self, param):
        return self.parameters[param]

    def set(self, handle, *value, **properties):
        '''
        Set the properties in `handle` according to the provided dictionary of
        properties and their values.

        For example: figure.set('ax[1].data', {'x': range(10), 'y': someData})

        :param handle: A string to the object or property of interest
        :param properties: A dictionary mapping properties to values or just the property values themselves. If `properties` is not a dictionary, Nutmeg will assume that `handle` points directly to a property.
        '''
        if not self.nutmeg.ready():
            self.sent = False

        self.send()
        if not self.sent:
            return

        self._setlock.acquire()
        fullHandle = self.handle + "." + handle
        value = (fullHandle,) + value

        def set_threaded(*args, **kwargs):
            try:
                self.nutmeg.set_values(*args, **kwargs)
                return
            except IOError as e:
                print("IOError in Nutmeg core...")
                print(e)
            finally:
                self._setlock.release()

        if self.assync:
            thread = threading.Thread(target=set_threaded, args=value, kwargs=properties)
            thread.daemon = False
            thread.start()
        else:
            set_threaded(*value, **properties)

        return

    def getParameterValues(self):
        '''
        :return: A dictionary of the parameters and their values.
        '''
        # TODO
        pass

    def registerUpdate(self, update, handle=""):
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


class Parameter():
    '''
    Keep track of a parameter's value and state. Currently, the parameter
    cannot be modified from Python.
    '''
    def __init__(self, name, value=0, changed=0):
        self.name = name
        self.value = value
        self._changed = changed
        self.callbacks = []
        self.valueLock = threading.Lock()
        self.changedLock = threading.Lock()

    @property
    def changed(self):
        return self._changed

    @changed.setter
    def changed(self, value):
        self.changedLock.acquire()
        self._changed = value
        if value == True:
            for callback in self.callbacks:
                callback()
        self.changedLock.release()

    def updateValue(self, value):
        self.valueLock.acquire()
        self.value = value
        self.changed += 1
        self.valueLock.release()

    def read(self):
        '''
        Return the value of the parameter and set the changed to False
        '''
        self.changed = 0
        return self.value

    def registerCallback(self, callback):
        '''
        Call this function whenever the value is changed.
        '''
        self.callbacks.append(callback)


class Updater():
    def __init__(self, params, init=None, update=None, threaded=False):
        '''
        :param params: A list of parameter names that this update depends on.
        '''
        self.paramNames = params
        self.init = init
        self.update = update
        self.initialized = False
        self.threaded = threaded
        # Figure and handle should be defined when the update is registered
        self.figure = None
        self.handle = ""
        self.parameters = {}

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
            print("Update waiting %s" % self.paramNames)
            self.updateEvent.wait()
            # Clear the event func and parameters to clean things up
            func, self.eventFunc = self.eventFunc, None
            self.updateEvent.clear()

            # Need to {param: value} pairs for the function updates.
            params = {p: self.parameters[p].value for p in self.parameters}

            print("Received event. Threaded %d" % self.threaded)

            if self.threaded:
                # If threaded, run in this thread
                result = func(params)
            else:
                # Else, start the update in a separate process.
                result = Parallel.sub_process(func, params)
                # print("Received result...")

            if self.figure and self.handle:
                self.figure.set(self.handle, result)

    def initializeParameters(self, params):
        # print("Initialise parameters for %s" % params)
        self.initialized = True
        # Register the Parameter objects
        for p in self.paramNames:
            # Add the parameter to own dict and register callback
            self.params[p] = params[p]
            self.params[p].registerCallback(lambda: self.parametersChanged())

        func = None
        if self.init is not None:
            func = self.init
        elif self.update is not None:
            func = self.update
        else:
            return
        if not self.updateEvent.isSet():
            self.eventFunc = func
            self.updateEvent.set()

    def parametersChanged(self):
        '''
        Trigger update
        '''
        print("Parameters changed")
        if self.update is None: return
        if not self.updateEvent.isSet():
            self.eventFunc = self.update
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
