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
        while (!ar[i]) ar[i] = (uint32_t) rand();
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
    char* buff = (char*) malloc(binaryMatrix->sls_for_line * sizeof(char));

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


bool check(struct BinaryMatrix *A, struct BinaryMatrix *B, struct BinaryMatrix *C, uint32_t *ar, uint32_t const *bits) {
    uint32_t *temp_ar = (uint32_t*) calloc(A->line_size, sizeof(uint32_t));
    uint32_t *left_ar = (uint32_t*) calloc(A->line_size, sizeof(uint32_t));
    uint32_t *right_ar = (uint32_t*) calloc(A->line_size, sizeof(uint32_t));

    mul(B, ar, temp_ar, bits);
    mul(A, temp_ar, left_ar, bits);
    mul(C, ar, right_ar, bits);

    for (uint32_t i = 0; i < A->line_size; ++i) {
        if (left_ar[i] != right_ar[i]) {
//            printf("%" SCNu32 " ", left_ar[i]);
//            printf("%" SCNu32 "\n", right_ar[i]);
            free(temp_ar);
            free(left_ar);
            free(right_ar);
            return true;
        }
    }
    
    free(temp_ar);
    free(left_ar);
    free(right_ar);
    return false;
}



int main() {
    srand(time(NULL));

    FILE *file;
    file = fopen("matrix.in", "r");

    if (!file) {
        return 1;
    }

    unsigned int n;
    fscanf(file, "%u", &n);


    struct BinaryMatrix A, B, C;
    create_BinaryMatrix(&A, n);
    create_BinaryMatrix(&B, n);
    create_BinaryMatrix(&C, n);

    init_BinaryMatrix(&A, file);
    init_BinaryMatrix(&B, file);
    init_BinaryMatrix(&C, file);

    uint32_t *bits = (uint32_t*) malloc(32 * sizeof(uint32_t));
    for (uint32_t i = 0; i < 32; ++i) {
        bits[i] = 1 << i;
    }


    uint32_t *ar = (uint32_t*) malloc(A.line_size * sizeof(uint32_t));

    uint32_t i = 0;
    for (i = 0; i < 52; ++i) {
        _random_vector(ar, A.line_size);
        for (uint32_t i = 0; i < A.line_size; ++i) {
            printf("%" SCNu32 " ", ar[i]);
        }
        printf("\n");
        if (check(&A, &B, &C, ar, bits)) break;
    }

    FILE *out = fopen("matrix.out", "w");
    fprintf(out, ((i == 52) ? "YES\n" : "NO\n"));
    fflush(out);

    fclose(out);
    fclose(file);

    free(bits);
    free(ar);

    delete_BinaryMatrix(&A);
    delete_BinaryMatrix(&B);
    delete_BinaryMatrix(&C);

    return 0;
}
