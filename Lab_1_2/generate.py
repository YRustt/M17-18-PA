#

if __name__ == '__main__':
    n = int(input())
    k = n // 4 + (1 if n % 4 else 0)

    with open("element.in", "w") as fout:

        for i in range(10000):
            fout.write("%s\n" % n)
            for i in range(3):
                for j in range(n):
                    fout.write("0" * k)
                    fout.write("\n" if j == n-1 else " ")

        fout.write("0")
