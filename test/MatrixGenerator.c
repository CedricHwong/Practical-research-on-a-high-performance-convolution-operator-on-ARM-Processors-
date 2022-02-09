
#include <stdio.h> // printf, fprintf
#include <stdlib.h> // malloc, free
#include <stdint.h> // >= c99; uint32_t
#include <time.h> // time

#define uint uint32_t

static uint LCG(uint seed) {
    return 214013u * seed + 2531011u;
}

#define GET_BIT(num, w, b) ((num) >> ((w) - 1u - (b)) & 1u)
#define GET_BIT_32(num, b) GET_BIT(num, 32u, b)
#define GET_IMG(pImgs, imgIndex, width) ((pImgs) + (imgIndex) * (width))
#define GET_IMG_32(pImgs, imgIndex) GET_IMG(pImgs, imgIndex, 32u)
#define IMG32ROW(pImgs, imgIndex, v) (*(GET_IMG_32(pImgs, imgIndex) + (v)))
// 读取像素
// 图片数组的指针，第几张图片，水平第几列[0, 32)，垂直第几行[0, 32)
#define IMG32PIXEL(pImgs, imgIndex, u, v) GET_BIT_32(IMG32ROW(pImgs, imgIndex, v), u)

/*
科普：图片uv坐标系
水平轴是u，向右为正方向；垂直轴是v，向下为正方向；原点在图片左上角
*/


// 生成大小为32x32的 个数为amount的 随机二值图像
static uint* genRand32x32Imgs(uint seed, uint amount) {
    // sizeof(uint) = 4
    uint *ans = (uint*)malloc(amount * 32u * sizeof(uint));
    if (ans) for (uint i = 0u; i < amount; ++i) {
        for (uint j = 0u; j < 32u; ++j) {
            *(ans + i * 32u + j) = seed;
            seed = LCG(seed);
        }
    }
    else
        // 使用keil的话，记得打开stderr的重定向
        fprintf(stderr, "Memory allocation failed!");
    return ans;
}

static void disposeImgs(uint **pImgs) {
    if (*pImgs == NULL) return;
    free(*pImgs);
    *pImgs = NULL;
}

// test
int main() {
    clock_t tic = clock();

    uint amount = 1000000;
    uint *pImgs = genRand32x32Imgs(23, amount);
    if (pImgs == NULL) return 0;
    for (uint a = 0u; a < amount; ++a) {
        printf("================================\n");
        for (uint v = 0u; v < 32u; ++v) {
            // uint num = *(pImgs + a * 32u + v);
            for (uint u = 0u; u < 32u; ++u) {
                // printf("%u", (num & (1u << u)) >> u);
                printf("%u", IMG32PIXEL(pImgs, a, u, v));
            }
            printf("\n");
        }
        printf("================================\n");
    }
    disposeImgs(&pImgs);
    clock_t toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    return 0;
}
