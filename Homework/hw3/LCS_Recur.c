#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int str1[] = {0,1, 2, 3, 5};
int str2[] = {2, 3,4,5};
int ans = 0;
int dp[10][10][10];
int DP(int i, int j, int count)
{
    if(dp[i][j][count]!=-1)
        return dp[i][j][count];
    if (i == 0 || j == 0)
    {
        dp[i][j][count] = 0;
        return 0;
    }
    if (str1[i-1] == str2[j-1])
    {
        if (count + 1 > ans)
            ans = count + 1;
        dp[i-1][j-1][count+1]=DP(i - 1, j - 1, count + 1);
    }
    else
    {
        dp[i-1][j][0]=DP(i - 1, j, 0);
        dp[i][j-1][0]=DP(i, j - 1, 0);
    }
    return dp[i][j][count];
}
int main()
{
    memset(dp, -1, sizeof(dp));
    DP(5, 4, 0);
    printf("%d\n", ans);
    return 0;
}