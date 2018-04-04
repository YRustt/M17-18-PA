//
// Created by yrustt on 4/4/18.
//

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <utility>


long long matrix[256][256 * 2];
int edges[256 * 256][2];

long long modpow(long long a, long long pow, long long mod) {
    a %= mod;
    long long result = 1LL;
    while (pow > 0) {
        if (pow & 1) {
            result = (result * a) % mod;
        }
        a = (a * a) % mod;
        pow >>= 1;
    }
    return result;
}


int main() {
    srand(time(0));

    int n, m, ie, je;
    int mod = 2147483647;
    FILE *fin = fopen("matching.in", "r");
    FILE *fout = fopen("matching.out", "w");

    fscanf(fin, "%d %d", &n, &m);
#ifdef DEBUG
    printf("%d %d\n", n, m);
#endif

    for (int i = 0; i < n; ++i) {
        matrix[i][i + n] = 1LL;
    }
    for (int i = 0; i < m; ++i) {
        fscanf(fin, "%d %d", &ie, &je);
        edges[i][0] = ie - 1;
        edges[i][1] = je - 1;
        matrix[ie - 1][je - 1] = rand() % (mod - 1) + 1;
        matrix[je - 1][ie - 1] = mod - matrix[ie - 1][je - 1];
    }

#ifdef DEBUG
    printf("<begin matrix>\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < (2 * n); ++j) {
            printf("%ld ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("<end matrix>\n");
#endif

    int sel = -1;
    for (int col = 0, row = 0; row < n && col < n; ++col) {
        sel = -1;
        for (int i = row; i < n; ++i) {
            if (matrix[i][col] != 0) {
                sel = i;
                break;
            }
        }
        if (sel == -1) {
            break;
        }
        for (int j = col; j < (2 * n); ++j) {
            std::swap(matrix[sel][j], matrix[row][j]);
        }

        for (int i = 0; i < n; ++i) {
            if (i != row) {
                long long c = (matrix[i][col] * modpow(matrix[row][col], mod - 2, mod)) % mod;
                for (int j = col; j < (2 * n); ++j) {
                    matrix[i][j] = (matrix[i][j] - (matrix[row][j] * c) % mod) % mod;
                }
            }
        }
#ifdef DEBUG
        printf("<begin matrix>\n");
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < (2 * n); ++j) {
                printf("%ld ", matrix[i][j]);
            }
            printf("\n");
        }
        printf("<end matrix>\n");
#endif
        ++row;
    }
    if (sel == -1) {
        for (int i = 0; i < m; ++i) {
            fprintf(fout, "NO\n");
        }
    } else {
        for (int i = 0; i < m; ++i) {
            int ie = edges[i][0];
            int je = edges[i][1];
            if (matrix[ie][je + n]) {
                fprintf(fout, "YES\n");
            } else {
                fprintf(fout, "NO\n");
            }
        }
    }

    return 0;
}