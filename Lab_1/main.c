//
// Created by yrustt on 10.2.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>


typedef uint32_t T;


struct BinaryMatrix {
    uint32_t size, line_size;
    uint32_t sls_for_line, sls_for_T;
    T* m;
};


T _round(const uint32_t a, const uint32_t b) {
    return a / b + ((a % b) ? 1 : 0);
}


bool create_BinaryMatrix(struct BinaryMatrix* binaryMatrix, uint32_t size) {
    binaryMatrix->size = size;
    binaryMatrix->sls_for_T = sizeof(T) / 4;
    binaryMatrix->sls_for_line = _round(binaryMatrix->size, 4);
    binaryMatrix->line_size = _round(binaryMatrix->sls_for_line, binaryMatrix->sls_for_T);
    binaryMatrix->m = malloc(binaryMatrix->size * binaryMatrix->line_size * sizeof(T));

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
    uint8_t* buff = malloc(binaryMatrix->sls_for_line * sizeof(uint8_t));

    for (uint32_t i = 0; i < binaryMatrix->size; ++i) {
        fscanf(file, "%s", buff);
        printf("%s\n", buff);
    }

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


    struct BinaryMatrix binaryMatrix;
    create_BinaryMatrix(&binaryMatrix, 6);

    init_BinaryMatrix(&binaryMatrix, file);


    delete_BinaryMatrix(&binaryMatrix);

    return 0;
}