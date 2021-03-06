import matplotlib.pyplot as plt


def transform_constraint(expression):
    aux = ""
    value = 0
    ret = [0, 0, 0]
    pos = False
    flag = True
    for c in expression:
        if c == '<' or c == '>' or (c == '=' and flag):
            index = int(aux)
            ret[index] = value
            aux = ""
            pos = False
            flag = False
        if c == 'x' or c == '+' or c == '-':
            if pos:
                index = int(aux)
                ret[index] = value
                pos = False
            elif len(aux) > 0:
                value = float(aux)
                pos = True
            aux = ""
        if '0' <= c <= '9' or c == '-':
            aux += c
            flag = True

    ret[2] = float(aux)

    return ret


def read_input(filename):
    ret = []

    fp = open(filename, "r")
    line = fp.readline()
    nVars = int(line)

    line = fp.readline()
    nConstrains = int(line)

    for i in range(0, nConstrains):
        ret.append(transform_constraint(fp.readline()))

    fp.close()

    return nVars, ret


def plot_constraints(constraints):
    for c in constraints:
        if c[0] != 0:
            x = c[2] / c[0]
        else:
            x = 0

        if c[1] != 0:
            y = c[2] / c[1]
        else:
            y = 0

        plt.plot((x, 0), (0, y), 'r-')


def read_output(filename):
    x = []
    y = []

    fp = open(filename)

    for line in fp:
        aux = line.rsplit(',')
        x.append(float(aux[0]))
        y.append(float(aux[1]))

    return x, y


def plot_points(x, y):
    plt.plot(x, y, 'b.')


if __name__ == "__main__":
    n, cons = read_input('in')
    if n == 2:
        plot_constraints(cons)

        x, y = read_output('out')
        plot_points(x, y)
        plt.axis()
        plt.show()
