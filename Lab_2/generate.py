import random
import math


MIN = 0
MAX = 10 ** 15


def is_prime(a):
    return all(a % i for i in range(2, int(math.sqrt(a))))


if __name__ == '__main__':
    n = int(input("Введите количество:\n"))

    with open("primality.in", "w") as fin, open("real_primality.out", "w") as fout:
        fin.write("%d\n" % n)

        for _ in range(n):
            x = random.randrange(MIN, MAX)

            while not x & 1:
                x = random.randrange(MIN, MAX)

            fin.write("%d\n" % x)
            fout.write("Yes\n" if is_prime(x) else "No\n")

            print(x)
