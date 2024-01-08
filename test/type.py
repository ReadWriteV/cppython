t = type(1)

print(t)

print(type(t))

a = int()
print(a, isinstance(a, object))

f = float()
print(f, isinstance(a, object))

b = str()
print(b, isinstance(b, object))

c = list()
print(c, isinstance(c, object))

d = dict()
print(d, isinstance(d, object))

i = 0
while i < 5:
    t = type(t)
    print(t)
    i = i + 1


class A(list):
    pass


e = A()
print(isinstance(e, list))
print(isinstance(e, A))
print(isinstance(e, int))
