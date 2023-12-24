class A(object):
    def __init__(self, v):
        self.value = v

    def __add__(self, a):
        return 100

    def __len__(self):
        return 200

    def __call__(self, s):
        print("call with", s)

    def __getitem__(self, s):
        if s == "hello":
            return "world"

    def __setitem__(self, k, v):
        print(k, v)


a = A(1)
b = A(2)

print(a + b)
print(len(a))
a("hello world")
print(a["hello"])
a[666] = "sixsixsix"


keys = []
values = []


class B(object):
    def __init__(self):
        print("create")

    def __setattr__(self, k, v):
        if k in keys:
            print("in")
            index = keys.index(k)
            print(index)
            values[index] = v
        else:
            print("setattr")
            keys.append(k)
            values.append(v)

    def __getattr__(self, k):
        if k in keys:
            index = keys.index(k)
            return values[index]
        else:
            return None


b = B()
b.foo = 1
b.bar = 2
print(b.foo)
print(b.bar)
b.foo = 3
print(b.foo)
