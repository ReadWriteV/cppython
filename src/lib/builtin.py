def map(func, iterable):
    l = []
    for i in iterable:
        l.append(func(i))

    return l


def filter(func, iterable):
    l = []
    for i in iterable:
        if func(i):
            l.append(i)

    return l


def sum(iterable, i):
    temp = i
    for e in iterable:
        temp = temp + e

    return temp


def range(*args):
    start = 0
    step = 1

    if len(args) == 1:
        end = args[0]
    elif len(args) == 2:
        start = args[0]
        end = args[1]
    else:
        start = args[0]
        end = args[1]
        step = args[2]

    lst = []
    if start < end and step > 0:
        while start < end:
            lst.append(start)
            start += step
    elif start > end and step < 0:
        while start > end:
            lst.append(start)
            start += step
    else:
        print("Error")

    return lst


class Exception(object):
    def __init__(self, *args):
        self.info = args

    def exc_str(self, exc_name):
        if len(self.info) == 0:
            return exc_name
        elif len(self.info) == 1:
            return exc_name + ": " + self.info[0].__repr__()
        else:
            return exc_name + ": " + self.info

    def __repr__(self):
        return self.exc_str("Exception")


class StopIteration(Exception):
    pass


class AssertionError(Exception):
    def __repr__(self):
        return self.exc_str("AssertionError")


def xrange(*alist):
    start = 0
    step = 1
    if len(alist) == 1:
        end = alist[0]
    elif len(alist) == 2:
        start = alist[0]
        end = alist[1]
    elif len(alist) == 3:
        start = alist[0]
        end = alist[1]
        step = alist[2]

    if start < end and step > 0:
        while start < end:
            yield start
            start += step
    elif start > end and step < 0:
        while start > end:
            yield start
            start += step
    else:
        raise StopIteration

    return
