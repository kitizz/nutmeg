

def med(s, e, lst=None):
    if lst is None:
        lst = []
    print "List:", lst

    if e-s == 2:
        lst.append(s)
        lst.append(s+1)
        return lst
    if e-s == 1:
        lst.append(s)
        return lst

    i = (e + s - 1)//2
    print s, e, i
    lst.append(i)

    med(s, i, lst)
    med(i+1, e, lst)

    return lst

print med(0, 8)
