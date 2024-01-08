i = 0
while i < 5:
    try:
        i += 1
        if i > 3:
            continue
        print(i)

    finally:
        print("hello")


def foo():
    try:
        return "haha"
    finally:
        print("world")


print(foo())

for i in range(5):
    try:
        if i < 3:
            continue
        print(i)
    finally:
        print("to be continued")
