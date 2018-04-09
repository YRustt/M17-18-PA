n = int(input())
with open("reachability.in", "w") as fout:
    fout.write("%d %d\n" % (n , n))
    for i in range(1, n):
        fout.write("%d %d\n" % (i, i + 1))
    fout.write("%d %d\n" % (n, 1))
