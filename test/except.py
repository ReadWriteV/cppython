try:
    raise StopIteration("hello")
except StopIteration as e:
    print("StopIteration", e)
except Exception as e:
    print("Exception", e)
finally:
    print("done")
