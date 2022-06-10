#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <emmintrin.h> /* where intrinsics are defined */

#define CLOCK_RATE_GHZ 2.0e9 /* just a wild guess ... */

/* Time stamp counter */
static __inline__ unsigned long long RDTSC(void)
{
    unsigned hi, lo;
    __asm__ volatile("rdtsc"
                     : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}

int sum_naive(int n, int *a)
{
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += a[i];
    return sum;
}

int sum_unrolled(int n, int *a)
{
    int sum = 0;

    /* do the body of the work in a faster unrolled loop */
    for (int i = 0; i < n / 4 * 4; i += 4)
    {
        sum += a[i + 0];
        sum += a[i + 1];
        sum += a[i + 2];
        sum += a[i + 3];
    }

    /* handle the small tail in a usual way */
    for (int i = n / 4 * 4; i < n; i++)
        sum += a[i];

    return sum;
}

int sum_vectorized(int n, int *a)
{
    /* VECTORIZE YOUR CODE HERE*/
    __m128i sum = _mm_setzero_si128();
    for (int i = 0; i < (n / 4) * 4; i += 4)
    {
        __m128i tmp = _mm_loadu_si128((__m128i *)(a + i));
        sum = _mm_add_epi32(sum, tmp);
    }
    int res[4];
    _mm_storeu_si128((__m128i *)(res), sum);
    int ans = 0;
    for (int i = 0; i < 4; ++i)
        ans += res[i];
    for (int i = (n / 4) * 4; i < n; ++i)
        ans += a[i];
    return ans;
}
int sum_vectorized_unrolled(int n, int *a)
{
    /* UNROLL YOUR VECTORIZED CODE HERE*/
    __m128i sum1 = _mm_setzero_si128();
    __m128i sum2 = _mm_setzero_si128();
    __m128i sum3 = _mm_setzero_si128();
    __m128i sum4 = _mm_setzero_si128();
    for (int i = 0; i < (n / 16) * 16; i += 16)
    {
        __m128i tmp0 = _mm_loadu_si128((__m128i *)(a + i + 0));
        sum1 = _mm_add_epi32(sum1, tmp0);
        __m128i tmp1 = _mm_loadu_si128((__m128i *)(a + i + 4));
        sum2 = _mm_add_epi32(sum2, tmp1);
        __m128i tmp2 = _mm_loadu_si128((__m128i *)(a + i + 8));
        sum3 = _mm_add_epi32(sum3, tmp2);
        __m128i tmp3 = _mm_loadu_si128((__m128i *)(a + i + 12));
        sum4 = _mm_add_epi32(sum4, tmp3);
    }
    int res1[4], res2[4], res3[4], res4[4];
    _mm_storeu_si128((__m128i *)(res1), sum1);
    _mm_storeu_si128((__m128i *)(res2), sum2);
    _mm_storeu_si128((__m128i *)(res3), sum3);
    _mm_storeu_si128((__m128i *)(res4), sum4);
    int ans = 0;
    for (int i = 0; i < 4; ++i)
    {
        ans += res1[i];
        ans += res2[i];
        ans += res3[i];
        ans += res4[i];
    }
    for (int i = (n / 16) * 16; i < n; ++i)
        ans += a[i];
    return ans;
}

void benchmark(int n, int *a, int (*computeSum)(int, int *), char *name)
{
    /* warm up */
    int sum = computeSum(n, a);

    /* measure */
    unsigned long long cycles = RDTSC();
    sum += computeSum(n, a);
    cycles = RDTSC() - cycles;

    double microseconds = cycles / CLOCK_RATE_GHZ * 1e6;

    /* report */
    printf("%20s: ", name);
    if (sum == 2 * sum_naive(n, a))
        printf("%.2f microseconds\n", microseconds);
    else
        printf("ERROR!\n");
}

int main(int argc, char **argv)
{
    const int n = 7777; /* small enough to fit in cache */

    /* init the array */
    int a[n] __attribute__((aligned(32))); /* align the array in memory by 32 bytes (good for 256 bit intrinsics) */
    for (int i = 0; i < n; i++)
        a[i] = rand();

    /* benchmark series of codes */
    benchmark(n, a, sum_naive, "naive");
    benchmark(n, a, sum_unrolled, "unrolled");
    benchmark(n, a, sum_vectorized, "vectorized");
    benchmark(n, a, sum_vectorized_unrolled, "vectorized unrolled");

    return 0;
}
