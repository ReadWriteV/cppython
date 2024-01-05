a = 1
b = 0

while a < 10:
    print(b)
    t = a
    a = a + b
    b = a
    if b > 5:
        break
