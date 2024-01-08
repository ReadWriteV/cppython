l = [1, 3, 1, 4]
s = [5, 2, 0]

print(l)

print(l.__repr__())

print(l + s)

print(s * 3)


l = [1, "hello"]
l = ["hello", "world"]

if "hello" in l:
    print("exist")
else:
    print("none")

print(l[1])

l = [4, 1, 2, 3]
l.reverse()
print(l)
