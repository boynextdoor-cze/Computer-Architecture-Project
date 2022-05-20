#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <algorithm>
#include <sys/time.h>
#include <time.h>
#include <immintrin.h>
#include <omp.h>
#include <avx2intrin.h>
// inplement dymanic

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PI 3.14159

typedef struct FVec
{
    unsigned int length;
    unsigned int min_length;
    unsigned int min_deta;
    float *data;
    float *sum;
} FVec;

typedef struct Image
{
    unsigned int dimX, dimY, numChannels;
    float *data;
} Image;

void normalize_FVec(FVec v)
{
    // float sum = 0.0;
    unsigned int i, j;
    int ext = v.length / 2;
    v.sum[0] = v.data[ext];
    for (i = ext + 1, j = 1; i < v.length; i++, j++)
        v.sum[j] = v.sum[j - 1] + v.data[i] * 2;
}

float *get_pixel(Image img, int x, int y)
{
    if (x < 0)
    {
        x = 0;
    }
    if (x >= img.dimX)
    {
        x = img.dimX - 1;
    }
    if (y < 0)
    {
        y = 0;
    }
    if (y >= img.dimY)
    {
        y = img.dimY - 1;
    }
    return img.data + 3 * (y * img.dimX + x);
}

float gd(float a, float b, float x)
{
    float c = (x - b) / a;
    return exp((-.5) * c * c) / (a * sqrt(2 * PI));
}

FVec make_gv(float a, float x0, float x1, unsigned int length, unsigned int min_length)
{
    FVec v;
    v.length = length;
    v.min_length = min_length;
    if (v.min_length > v.length)
    {
        v.min_deta = 0;
    }
    else
    {
        v.min_deta = ((v.length - v.min_length) / 2);
    }
    v.data = malloc(length * sizeof(float));
    v.sum = malloc((length / 2 + 1) * sizeof(float));
    float step = (x1 - x0) / ((float)length);
    int offset = length / 2;

    for (int i = 0; i < length; i++)
    {
        v.data[i] = gd(a, 0.0f, (i - offset) * step);
    }
    normalize_FVec(v);
    return v;
}

void print_fvec(FVec v)
{
    unsigned int i;
    printf("\n");
    for (i = 0; i < v.length; i++)
    {
        printf("%f ", v.data[i]);
    }
    printf("\n");
}

Image img_sc(Image a)
{
    Image b = a;
    b.data = malloc(b.dimX * b.dimY * 3 * sizeof(float));
    return b;
}

unsigned int min(unsigned int a, unsigned int b)
{
    return a < b ? a : b;
}

Image gb_h(Image a, FVec gv, float *flatten)
{
    Image b = img_sc(a);
    int ext = gv.length / 2;

    float *pixel = (float *)malloc((a.dimX + 2 * ext) * a.dimY * 3 * sizeof(float));

    // Pre-process marginal pixels
#pragma omp parallel for schedule(dynamic)
    for (unsigned int y = 0; y < a.dimY; ++y)
        for (int x = -ext; x < (int)a.dimX + ext; ++x)
        {
            float *tmp = get_pixel(a, x, y);
            pixel[3 * (y * (a.dimX + 2 * ext) + x + ext) + 0] = tmp[0];
            pixel[3 * (y * (a.dimX + 2 * ext) + x + ext) + 1] = tmp[1];
            pixel[3 * (y * (a.dimX + 2 * ext) + x + ext) + 2] = tmp[2];
        }

#pragma omp parallel for schedule(dynamic)
    for (unsigned int y = 0; y < a.dimY; ++y)
        for (unsigned int x = 0; x < a.dimX; ++x)
        {
            float *pc = get_pixel(b, x, y);
            unsigned int deta = min(min(a.dimY - y - 1, y), min(a.dimX - x - 1, x));
            deta = min(deta, gv.min_deta);
            __m256 sum1 = _mm256_setzero_ps();
            __m256 sum2 = _mm256_setzero_ps();
            __m256 sum3 = _mm256_setzero_ps();
            int i;
            for (i = deta; i < gv.length - deta - 8; i += 8)
            {
                __m256 pix = _mm256_loadu_ps(pixel + 3 * (y * (a.dimX + 2 * ext) + x + i));
                __m256 gv_data = _mm256_loadu_ps(flatten + 3 * i);
                sum1 = _mm256_fmadd_ps(pix, gv_data, sum1);

                pix = _mm256_loadu_ps(pixel + 3 * (y * (a.dimX + 2 * ext) + x + i + 2) + 2);
                gv_data = _mm256_loadu_ps(flatten + 3 * (i + 2) + 2);
                sum2 = _mm256_fmadd_ps(pix, gv_data, sum2);

                pix = _mm256_loadu_ps(pixel + 3 * (y * (a.dimX + 2 * ext) + x + i + 5) + 1);
                gv_data = _mm256_loadu_ps(flatten + 3 * (i + 5) + 1);
                sum3 = _mm256_fmadd_ps(pix, gv_data, sum3);
            }
            float res1[8], res2[8], res3[8];
            _mm256_storeu_ps(res1, sum1);
            _mm256_storeu_ps(res2, sum2);
            _mm256_storeu_ps(res3, sum3);
            float sum_r = res1[0] + res1[3] + res1[6] + res2[1] + res2[4] + res2[7] + res3[2] + res3[5];
            float sum_g = res1[1] + res1[4] + res1[7] + res2[2] + res2[5] + res3[0] + res3[3] + res3[6];
            float sum_b = res1[2] + res1[5] + res2[0] + res2[3] + res2[6] + res3[1] + res3[4] + res3[7];
            for (; i < gv.length - deta; ++i)
            {
                int offset = i - ext;
                sum_r += gv.data[i] * get_pixel(a, x + offset, y)[0];
                sum_g += gv.data[i] * get_pixel(a, x + offset, y)[1];
                sum_b += gv.data[i] * get_pixel(a, x + offset, y)[2];
            }
            pc[0] = sum_r / gv.sum[ext - deta];
            pc[1] = sum_g / gv.sum[ext - deta];
            pc[2] = sum_b / gv.sum[ext - deta];
        }

    free(pixel);
    return b;
}

void expand(Image *img)
{
    img->data = (float *)realloc(img->data, (img->dimX * img->dimY * 3 + 50) * sizeof(float));
}

void transpose(Image *src, Image *dst)
{
    int len = src->dimX * src->dimY * 3;
    dst->dimX = src->dimY;
    dst->dimY = src->dimX;
    dst->numChannels = 3;
    dst->data = (float *)malloc(len * sizeof(float));
#pragma omp parallel for schedule(dynamic)
    for (unsigned int x = 0; x < src->dimX; ++x)
        for (unsigned int y = 0; y < src->dimY; ++y)
        {
            dst->data[3 * (x * dst->dimX + y) + 0] = src->data[3 * (y * src->dimX + x) + 0];
            dst->data[3 * (x * dst->dimX + y) + 1] = src->data[3 * (y * src->dimX + x) + 1];
            dst->data[3 * (x * dst->dimX + y) + 2] = src->data[3 * (y * src->dimX + x) + 2];
        }
}

Image apply_gb(Image a, FVec gv)
{
    float *flatten_data = (float *)malloc((gv.length * 3 + 50) * sizeof(float));
#pragma omp parallel for schedule(dynamic)
    for (unsigned int i = 0; i < gv.length; ++i)
    {
        flatten_data[3 * i] = gv.data[i];
        flatten_data[3 * i + 1] = gv.data[i];
        flatten_data[3 * i + 2] = gv.data[i];
    }
    Image b = gb_h(a, gv, flatten_data);
    Image b_trans;
    transpose(&b, &b_trans);
    free(b.data);
    expand(&b_trans);

    Image c = gb_h(b_trans, gv, flatten_data);
    free(b_trans.data);
    Image res;
    transpose(&c, &res);
    free(c.data);

    free(flatten_data);
    return res;
}

int main(int argc, char **argv)
{
    struct timeval start_time, stop_time, elapsed_time;
    gettimeofday(&start_time, NULL);
    if (argc < 6)
    {
        printf("Usage: ./gb.exe <inputjpg> <outputname> <float: a> <float: x0> <float: x1> <unsigned int: dim>\n");
        exit(0);
    }

    float a, x0, x1;
    unsigned int dim, min_dim;

    sscanf(argv[3], "%f", &a);
    sscanf(argv[4], "%f", &x0);
    sscanf(argv[5], "%f", &x1);
    sscanf(argv[6], "%u", &dim);
    sscanf(argv[7], "%u", &min_dim);

    FVec v = make_gv(a, x0, x1, dim, min_dim);
    // print_fvec(v);
    Image img;
    img.data = stbi_loadf(argv[1], &(img.dimX), &(img.dimY), &(img.numChannels), 0);

    expand(&img);

    Image imgOut = apply_gb(img, v);
    stbi_write_jpg(argv[2], imgOut.dimX, imgOut.dimY, imgOut.numChannels, imgOut.data, 90);
    gettimeofday(&stop_time, NULL);
    timersub(&stop_time, &start_time, &elapsed_time);
    printf("%f \n", elapsed_time.tv_sec + elapsed_time.tv_usec / 1000000.0);
    free(imgOut.data);
    free(v.data);
    free(v.sum);
    return 0;
}
