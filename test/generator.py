def fib(n):
    i = 0
    a = 1
    b = 1
    while i < n:
        yield a
        i += 1
        t = a
        a = a + b
        b = t

    return


for i in fib(10):
    print(i)
