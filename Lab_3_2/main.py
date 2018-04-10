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
    # type: (int, int, int, int) -> int

    m = pow(a, v, num)

    if m == 1:
        return 2

    for _ in range(u):
        if m == num - 1:
            return 2

        m = (m * m) % num

    return 1 if m == 1 else 0


def karmaikle(num):
    # type: (int) -> bool

    it_c = 25
    v, u = _calc_v_u(num)
    flag = False

    while it_c >= 0:
        a = random.randrange(2, num)
        if math.gcd(a, num) == 1:
            res = _check(a, v, u, num)

            if res == 0:
                return False

            if res == 1:
                flag = True

            it_c -= 1
        else:
            flag = True

    return flag


if __name__ == "__main__":
    with open("carmichael.in", "r") as fin, open("carmichael.out", "w") as fout:
        t = fin.readline().strip()
        for line in fin:
            p = int(line)
            if p < 561:
                fout.write("No\n")
            elif not p & 1:
                fout.write("No\n")
            elif karmaikle(p):
                fout.write("Yes\n")
            else:
                fout.write("No\n")
