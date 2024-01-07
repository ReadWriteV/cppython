try:
    raise Exception("something wrong")
except Exception as e:
    print(e)
finally:
    print("done")
