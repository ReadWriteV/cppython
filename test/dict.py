d = {1: "hello", "world": 2}

print(d)

for k in d:
    print(k)

for k in d.keys():
    print(k)

for v in d.values():
    print(v)

for k_v in d.items():
    print(k_v)

for k, v in d.items():
    print(k, v)


print(d[1])
print(d["world"])

d.setdefault(1, 2)
d.setdefault(2, 3)

print(d[1])
print(d[2])
del d[1]
print(d)
d.pop(2)
print(d)
