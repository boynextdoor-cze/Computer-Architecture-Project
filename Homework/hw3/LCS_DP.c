#include <stdio.h>
#include <stdlib.h>
int s1[] = {0,1,2,3,4,5};
int s2[] = {1,3,4,5};
int len1 = 6, len2 = 4, ans = 0;
int max(int a,int b)
{
    if(a>=b)
        return a;
    return b;
}
int main()
{
    int i, j;
    for (i = 0; i < len1; i++)
    {
        for (j = 0; j < len2; j++)
        {
            int k = 0;
            int len = 0;
            while(i+k<len1&&j+k<len2&&s1[i+k]==s2[j+k])
            {
                k++;
                len++;
            }
            ans = max(len, ans);
        }
    }
    return 0;
}