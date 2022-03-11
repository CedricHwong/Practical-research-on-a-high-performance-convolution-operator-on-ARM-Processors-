
#define OUTPUT_GEN_RAND_IMG_TIME

#include <stdio.h> // printf, fprintf
#include <stdlib.h> // malloc, free
#include <stdint.h> // >= c99; uint32_t
#include <time.h> // time

#define uint uint32_t

// // Source: Numerical Recipes - LCG(1664525, 1013904223, 2^32)
// static inline uint LCG(uint seed) { return 1664525u * seed + 1013904223u; }
// // Source: Based on the theory mentioned in TAOCP, change m to make the lower bits random. But there is no strict choice of a and c.
// static inline uint LCG(uint seed) { return (uint)((1664525ul * (uint64_t) seed + 1013904223ul) % 0x100000001ul); }
// static inline uint LCG(uint seed) { return (uint)((1664525ul * (uint64_t) seed + 1013904223ul) % 0xFFFFFFFFul); }
// // Source: Microsoft Visual/Quick C/C++ - LCG(214013, 2531011, 2^32)
// static inline uint LCG(uint seed) { return 214013u * seed + 2531011u; }

static inline uint xorshift(uint seed) {
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

// #define RAND LCG
#define RAND xorshift

#define GET_BIT(num, w, b) ((num) >> ((w) - 1u - (b)) & 1u)
#define GET_BIT_32(num, b) GET_BIT(num, 32u, b)
#define GET_IMG(pImgs, imgIndex, width) ((pImgs) + (imgIndex) * (width))
#define GET_IMG_32(pImgs, imgIndex) GET_IMG(pImgs, imgIndex, 32u)
#define IMG32ROW(pImgs, imgIndex, v) (*(GET_IMG_32(pImgs, imgIndex) + (v)))
// 读取像素
// 图片数组的指针，第几张图片，水平第几列[0, 32)，垂直第几行[0, 32)
#define IMG32PIXEL(pImgs, imgIndex, u, v) GET_BIT_32(IMG32ROW(pImgs, imgIndex, v), u)

// IMG32PIXEL(pImgs, a, u, v)
// pImgs[a][u, v]

// 锐化的卷积核3*3
/*
  0  1  0
  1 -4  1
  0  1  0
*/
/*
  0 -1  0
 -1  5 -1
  0 -1  0
*/
// 5*5
/*
  0  0  0  0  0
  0  0 -1  0  0
  0 -1  5 -1  0
  0  0 -1  0  0
  0  0  0  0  0
*/
int k_3[3][3] = {
    {0, 1, 0},
    {1, -4, 1},
    {0, 1, 0}
};
int k_5[5][5] = {
    {0, 0, 0, 0, 0},
    {0, 0, -1, 0, 0},
    {0, -1, 5, -1, 0},
    {0, 0, -1, 0, 0},
    {0, 0, 0, 0, 0}
};

int** createKernel(int size){
    if (size == 3){
        return k_3;
    } else if (size == 5){
        return k_5;
    } else {
        fprintf(stderr, "Wrong kernel size");
        return NULL;
    }
}

// 生成大小为32x32的 个数为amount的 随机二值图像
static uint* genRand32x32Imgs(uint seed, uint amount) {
    // sizeof(uint) = 4
    uint *ans = (uint*)malloc(amount * 32u * sizeof(uint));
    if (ans == NULL) {
        // If you use keil, remember to open stderr redirection
        fprintf(stderr, "Memory allocation failed!");
        return NULL;
    }
#ifdef OUTPUT_GEN_RAND_IMG_TIME
    // If the amount of data used for testing is too large, frequent memory swaps will occur,
    // resulting in a slowdown in execution speed.
    // So output the size here for judgment.
    printf("amount = %u (= %llu Bytes = %f KB = %f MB = %f GB)\n", amount,
        amount * 32u * sizeof(uint),
        (double)(amount * 32u * sizeof(uint)) / 1024.0,
        (double)(amount * 32u * sizeof(uint)) / 1024.0 / 1024.0,
        (double)(amount * 32u * sizeof(uint)) / 1024.0 / 1024.0 / 1024.0
    );
    clock_t tic = clock();
#endif
    seed = RAND(seed);
    for (uint i = 0u; i < amount; ++i) {
        for (uint j = 0u; j < 32u; ++j) {
            seed = RAND(seed);
            *(ans + i * 32u + j) = seed;
        }
    }
#ifdef OUTPUT_GEN_RAND_IMG_TIME
    clock_t toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
#endif
    return ans;
}

static void disposeImgs(uint **pImgs) {
    if (*pImgs == NULL) return;
    free(*pImgs);
    *pImgs = NULL;
}

// test
int main() {
    uint amount = 130000000;
    uint *pImgs = genRand32x32Imgs(23, amount);
    if (pImgs == NULL) return 0;

    int size = 3;
    int** k_ = createKernel(size);
    
    if (k_ == NULL) { return 0; }

    // IMG32PIXEL(pImgs, a, u, v)
    // pImgs[a][u, v]


    
    for (uint a = 0u; a < amount; ++a) {
        // printf("+================================+\n|");
        for (uint v = 0u; v < 32u - size + 1; ++v) {
            // uint num = *(pImgs + a * 32u + v);
            for (uint u = 0u; u < 32u - size + 1; ++u) {
                // printf("%u", (num & (1u << u)) >> u);
                // printf("%c", IMG32PIXEL(pImgs, a, u, v)? 'H': ' ');
                
                int count = 0;
                // each node in kernel
                for (int h = 0; h < size; h++) {
                    for (int w = 0; w < size; w++) {
                        // account the output value
                        count += IMG32PIXEL(pImgs, a, u, v) * k_[h][w];
                    }
                }
                // now, count is the output node of the kernel
            }
            // if (v == 31u)printf("|\n");
            // else printf("|\n|");
        }
        // printf("+================================+\n");
    }
    disposeImgs(&pImgs);
    return 0;
}
