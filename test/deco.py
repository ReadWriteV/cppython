def call_cnt(fn):
    cnt = [
        0,
    ]

    def inner_func(a, b):
        cnt[0] += 1
        print(cnt[0])
        return fn(a, b)

    return inner_func


@call_cnt
def add(a, b=2):
    return a + b


print(add(1, 2))
print(add(2, 3))
