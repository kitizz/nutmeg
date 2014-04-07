

class Update:

    def __init__(self, func):
        self.func = func

    def trigger(self, values):
        return self.func(values)

def myFunc(values):
    print values
    return values.keys()


ud = Update(myFunc)

print ud.trigger({"a":45, "b":"world"})
