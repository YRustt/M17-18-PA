import math
import random


def _calc_it_count(num):
    # type: (int) -> int

    return 2 * int(math.ceil(math.log2(num)))


def _calc_v_u(num):
    # type: (int) -> (int, int)

    v, u = num - 1, 0

    while not v & 1:
        v >>= 1
        u += 1

    return v, u


def _check(a, v, u, num):
    # type: (int, int, int, int) -> bool

    m = pow(a, v, num)

    if m == 1:
        return True

    for _ in range(u - 1):
        if m == num - 1:
            return True

        m = (m * m) % num

    return m == num - 1


def miller_rabin(num):
    # type: (int) -> bool

    it_c = 30
    v, u = _calc_v_u(num)

    for _ in range(it_c):
        a = random.randrange(2, num - 1)
        if not _check(a, v, u, num):
            return False

    return True


if __name__ == "__main__":
    with open("primality.in", "r") as fin, open("primality.out", "w") as fout:
        t = fin.readline().strip()
        for line in fin:
            p = int(line)
            if p in (0, 1):
                fout.write("No\n")
            elif p in (2, 3):
                fout.write("Yes\n")
            elif not p & 1:
                fout.write("No\n")
            elif miller_rabin(p):
                fout.write("Yes\n")
            else:
                fout.write("No\n")
