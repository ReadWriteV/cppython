class T:
    def __init__(self):
        self.color = "red"

    def __repr__(self):
        return self.color + "__repr__"

    def __str__(self):
        return self.color + "__str__"


t = T()
print(t)
print(repr(t))
