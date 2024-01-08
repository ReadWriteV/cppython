l = [1, 3, 1, 4]
s = [5, 2, 0]

print(l)

a = l.__repr__()

print(l + s)

print(s * 3)


class A(list):
    pass


a = A()
a.append(3)
print(a[0])
print(a)
