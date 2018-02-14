//
// Created by yrustt on 10.2.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>


uint32_t _round(const uint32_t a, const uint32_t b) {
    return a / b + ((a % b) ? 1 : 0);
}


uint32_t _get_code(char s) {
    if ((s >= '0') && (s <= '9')) return (uint32_t) (s - '0');
    return (uint32_t) (s - 'A');
}


uint32_t _get_number(uint32_t a[8]) {
    return (a[0] << 28) + (a[1] << 24) + (a[2] << 20) + (a[3] << 16) + (a[4] << 12) + (a[5] << 8) + (a[6] << 4) + a[7];
}


struct BinaryMatrix {
    uint32_t size, line_size;
    uint32_t sls_for_line, sls_for_T;
    uint32_t *m;
};


bool create_BinaryMatrix(struct BinaryMatrix* binaryMatrix, uint32_t size) {
    binaryMatrix->size = size;
    binaryMatrix->sls_for_T = sizeof(uint32_t) / 4;
    binaryMatrix->sls_for_line = _round(binaryMatrix->size, 4);
    binaryMatrix->line_size = _round(binaryMatrix->sls_for_line, binaryMatrix->sls_for_T);
    binaryMatrix->m = malloc(binaryMatrix->size * binaryMatrix->line_size * sizeof(uint32_t));

    return binaryMatrix->m;
}


void delete_BinaryMatrix(struct BinaryMatrix* binaryMatrix) {
    binaryMatrix->size = 0;
    binaryMatrix->line_size = 0;
    binaryMatrix->sls_for_T = 0;
    binaryMatrix->sls_for_line = 0;

    free(binaryMatrix->m);
}


bool init_BinaryMatrix(struct BinaryMatrix* binaryMatrix, FILE* file) {
    char* buff = malloc(binaryMatrix->sls_for_line * sizeof(char));

    for (uint32_t i = 0; i < binaryMatrix->size; ++i) {
        fscanf(file, "%s", buff);

        uint32_t a[8];

        for (uint32_t j = 0; j < binaryMatrix->sls_for_line; j += 8) {
            for (uint32_t k = 0; k < 8; ++k) {
                a[k] = (j + k < binaryMatrix->sls_for_line)? _get_code(buff[j + k]) : 0;
            }
            binaryMatrix->m[i * binaryMatrix->line_size + j / 8] = _get_number(a);
        }
    }

    free(buff);
    return true;
}




int main() {
    FILE *file;
    file = fopen("matrix.in", "r");

    if (!file) {
        return 1;
    }

    uint32_t n;
    fscanf(file, "%" SCNu32, &n);


    struct BinaryMatrix A, B, C;
    create_BinaryMatrix(&A, n);
    create_BinaryMatrix(&B, n);
    create_BinaryMatrix(&C, n);

    init_BinaryMatrix(&A, file);
    init_BinaryMatrix(&B, file);
    init_BinaryMatrix(&C, file);


    delete_BinaryMatrix(&A);
    delete_BinaryMatrix(&B);
    delete_BinaryMatrix(&C);

    return 0;
}