t = type(1)

print(t)

print(type(t))

a = int()
print(a)

b = str()
print(b)

c = list()
print(c)

d = dict()
print(d)


class A(list):
    pass


e = A()
print(isinstance(e, list))
print(isinstance(e, A))
print(isinstance(e, int))
