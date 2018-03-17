//
// Created by yrustt on 10.2.18.
//

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>


uint32_t _round(const uint32_t a, const uint32_t b) {
    return a / b + ((a % b) ? 1 : 0);
}


uint32_t _get_code(char s) {
    return (uint32_t) (((s >= '0') && (s <= '9')) ? (s - '0') : (s - 'A' + 10));
}


uint32_t _get_number(uint32_t *a) {
    uint32_t res = 0;
    for (uint32_t i = 0; i < 32; i += 4) {
        res |= (a[i / 4] << (32 - i - 4));
//        printf("%" SCNu32 " ", BIT_SIZE - i - 4);
    }
    return res;
}


void _random_vector(uint32_t *ar, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        ar[i] = 0;
        uint32_t x = 0;
        while (!ar[i]) {
            x = rand() & 0xff;
            x |= (rand() & 0xff) << 8;
            x |= (rand() & 0xff) << 16;
            x |= (rand() & 0xff) << 24;
            ar[i] = x;
        }
    }
}


struct BinaryMatrix {
    uint32_t size, line_size;
    uint32_t sls_for_line, sls_for_T;
    uint32_t *m;
};


bool create_BinaryMatrix(struct BinaryMatrix *binaryMatrix, uint32_t size) {
    binaryMatrix->size = size;
    binaryMatrix->sls_for_T = 8;
    binaryMatrix->sls_for_line = _round(binaryMatrix->size, 4);
    binaryMatrix->line_size = _round(binaryMatrix->sls_for_line, binaryMatrix->sls_for_T);
    binaryMatrix->m = (uint32_t*) malloc(binaryMatrix->size * binaryMatrix->line_size * sizeof(uint32_t));

    return binaryMatrix->m;
}


void delete_BinaryMatrix(struct BinaryMatrix *binaryMatrix) {
    binaryMatrix->size = 0;
    binaryMatrix->line_size = 0;
    binaryMatrix->sls_for_T = 0;
    binaryMatrix->sls_for_line = 0;

    free(binaryMatrix->m);
}


bool init_BinaryMatrix(struct BinaryMatrix* binaryMatrix, FILE* file) {
    char* buff = (char*) malloc(binaryMatrix->sls_for_line * sizeof(char) + 1);

    for (uint32_t i = 0; i < binaryMatrix->size; ++i) {
        fscanf(file, "%s", buff);

        uint32_t a[binaryMatrix->sls_for_T];

        for (uint32_t j = 0; j < binaryMatrix->sls_for_line; j += binaryMatrix->sls_for_T) {
            for (uint32_t k = 0; k < binaryMatrix->sls_for_T; ++k) {
                a[k] = (j + k < binaryMatrix->sls_for_line)? _get_code(buff[j + k]) : 0;
//                printf("%" SCNu32 " ", a[k]);
            }
            binaryMatrix->m[i * binaryMatrix->line_size + j / binaryMatrix->sls_for_T] = _get_number(a);
//            printf("%" SCNu32 " ", binaryMatrix->m[i * binaryMatrix->line_size + j / binaryMatrix->sls_for_T]);
        }
//        printf("\n");
    }

    free(buff);
    return true;
}


void mul(struct BinaryMatrix *A, uint32_t *ar, uint32_t *res, uint32_t const *bits) {
    for (uint32_t i = 0; i < A->size; i += 32) {
        res[i / 32] = 0;
        uint32_t tmp = 0;
        for (uint32_t j = 0; j < 32; ++j) {
            if ((i + j) >= A->size) break;

            uint32_t b = 0;
            for (uint32_t k = 0; k < A->line_size; ++k) {
                b ^= A->m[(i + j) * A->line_size + k] & ar[k];
            }

            uint32_t count = 0;
            for (uint32_t k = 0; k < 32; ++k) {
                if (b & bits[k]) {
                    count++;
                }
            }
//            printf("%" SCNu32 " ", count % 2);

            if (count % 2) {
                tmp |= bits[31 - j];
            }
        }
//        printf("%" SCNu32 "\n", tmp);
        res[i / 32] = tmp;
    }
}


int32_t check_ar(uint32_t *a, uint32_t *temp, uint32_t *c, uint32_t *ar, uint32_t line_size, uint32_t const *bits) {
    for (uint32_t i = 0; i < line_size; ++i) {
        uint32_t left = a[i] & temp[i];
        uint32_t right = c[i] & ar[i];
//        printf("%" SCNu32 " ", i);
//        printf("%" SCNu32 " ", left);
//        printf("%" SCNu32 "\n", right);
        if (left != right) {
            for (uint32_t j = 0; j < 32; ++j) {
                if ((left & bits[j]) != (right & bits[j])) {
                    return i * 32 + 32 - j;
                }
            }
        }
    }
    return -1;
}


int32_t check(struct BinaryMatrix *A, struct BinaryMatrix *B, struct BinaryMatrix *C, uint32_t *ar, uint32_t const *bits) {
    uint32_t *temp_ar = (uint32_t*) calloc(A->line_size, sizeof(uint32_t));
    uint32_t *left_ar = (uint32_t*) calloc(A->line_size, sizeof(uint32_t));
    uint32_t *right_ar = (uint32_t*) calloc(A->line_size, sizeof(uint32_t));

    mul(B, ar, temp_ar, bits);
    mul(A, temp_ar, left_ar, bits);
    mul(C, ar, right_ar, bits);

    uint32_t i = 0;
    for (i = 0; i < A->line_size; ++i) {
        if (left_ar[i] != right_ar[i]) break;
    }

    if (i == A->line_size) {
        free(temp_ar);
        free(left_ar);
        free(right_ar);
        return -1;
    } else {
        for (uint32_t j = 0; j < 32; ++j) {
            if ((left_ar[i] & bits[j]) != (right_ar[i] & bits[j])) {
                int32_t i_ = i * 32 + 32 - j;
//                printf("%" SCNu32 " ", A->m[i_ * A->line_size - 1]);
//                printf("%" SCNu32 "\n", C->m[i_ * A->line_size - 1]);
                int32_t j_ = check_ar(&A->m[(i_ - 1) * A->line_size], temp_ar, &C->m[(i_ - 1) * A->line_size], ar, A->line_size, bits);
                free(temp_ar);
                free(left_ar);
                free(right_ar);
                return i_ * 100000 + j_;
            }
        }
    }
}



int main() {
    srand(time(NULL));

    FILE *file = fopen("element.in", "r");
    FILE *out = fopen("element.out", "w");

    unsigned int n;
    uint32_t *bits = (uint32_t *) malloc(32 * sizeof(uint32_t));
    for (uint32_t i = 0; i < 32; ++i) {
        bits[i] = 1 << i;
    }

    while (true) {
        fscanf(file, "%u", &n);

        if (n == 0) {
            break;
        }

        struct BinaryMatrix A, B, C;
        create_BinaryMatrix(&A, n);
        create_BinaryMatrix(&B, n);
        create_BinaryMatrix(&C, n);

        init_BinaryMatrix(&A, file);
        init_BinaryMatrix(&B, file);
        init_BinaryMatrix(&C, file);

        uint32_t *ar = (uint32_t *) malloc(A.line_size * sizeof(uint32_t));

        uint32_t i = 0;
        int32_t idx = -1;
        for (i = 0; i < 52; ++i) {
            _random_vector(ar, A.line_size);
            idx = check(&A, &B, &C, ar, bits);
            if (idx != -1) {
                fprintf(out, "Yes\n%d %d\n", idx / 100000, idx % 100000);
                break;
            }
        }
        if (idx == -1) {
            fprintf(out, "No\n");
        }

        free(ar);

        delete_BinaryMatrix(&A);
        delete_BinaryMatrix(&B);
        delete_BinaryMatrix(&C);
    }

    free(bits);
    fclose(out);
    fclose(file);

    return 0;
}
