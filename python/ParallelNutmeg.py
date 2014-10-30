import multiprocessing
import signal
import sys


def _spawn(f):
    def fun(q_in,q_out):
        while True:
            i,x = q_in.get()
            if i is None:
                break
            q_out.put((i,f(x)))
    return fun

proc = []
original_sigint = None

def parallelize(func, argList, maxProcesses=multiprocessing.cpu_count()):
    '''
    Parellize multiple calls to `func` with values from `argList`.

    Ref: http://stackoverflow.com/a/16071616/1512137

    :param func: A function which takes a single argument.
    :param argList: A single dimension array-like object.
    :param maxProcesses: The maximum allowed threads. If set to -1, the maxProcesses will be set to the length of arg-list.

    :return: List of return values associated with argList.
    '''
    # store the original SIGINT handler
    global original_sigint
    original_sigint = signal.getsignal(signal.SIGINT)
    signal.signal(signal.SIGINT, exit_gracefully)

    q_in = multiprocessing.Queue(1)
    q_out = multiprocessing.Queue()

    global proc
    proc = [multiprocessing.Process(target=_spawn(func), args=(q_in,q_out))
            for _ in range(maxProcesses)]
    for p in proc:
        p.daemon = True
        p.start()

    sent = [q_in.put((i,x)) for i,x in enumerate(argList)]
    [q_in.put((None,None)) for _ in range(maxProcesses)]
    res = [q_out.get() for _ in range(len(sent))]

    [p.join() for p in proc]

    resultArray = [x for i,x in sorted(res)]
    if type(resultArray[0]) is tuple:
        newResult = []
        for i in range(len(resultArray[0])):
            newResult.append( [v[i] for v in resultArray] )
        resultArray = tuple(newResult)

    return resultArray

def _spawn_single(f):
    def fun(x,q_out):
        q_out.put(f(x))
    return fun

def sub_process(func, arg):
    q_out = multiprocessing.Queue(1)

    p = multiprocessing.Process(target=_spawn_single(func), args=(arg, q_out))
    p.start()
    result = q_out.get()
    p.join()
    return result


def exit_gracefully(signum, frame):
    # restore the original signal handler as otherwise evil things will happen
    # in raw_input when CTRL+C is pressed, and our signal handler is not re-entrant
    signal.signal(signal.SIGINT, original_sigint)

    print('Killing subprocesses before exit...')
    for p in proc:
        p.terminate()
    sys.exit(0)
    # restore the exit gracefully handler here    
    signal.signal(signal.SIGINT, exit_gracefully)


def _quickTest():
    x = range(7,0,-1)
    f = lambda y: y**2

    def waitTime(x):
        import time
        print 'Sleeping:', x
        time.sleep(x/2)
        return x
    print parallelize(waitTime, x, maxProcesses=8)

if __name__ == '__main__':
    _quickTest()
