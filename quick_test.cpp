/*************************************************************************
    > File Name: quick_test.cpp
    > Author: Herixth
    > Mail: herixth@gmail.com 
    > Created Time: 2019年01月15日 17:57:15
 ************************************************************************/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <string>
using namespace std;

typedef unsigned int UI;
typedef pair<float, UI> pis;

const int maxn = 5000000;
pis arr[maxn];

void rand_data(int n) {
    for (int inc = 0; inc < n; inc ++) {
        arr[inc] = pis(rand() % 1000 / 11, rand() % 10000);
    }
}

int main() {
    double ans = 0;
    srand(time(NULL));
    for (int inc = 0; inc < 100; inc ++) {
    rand_data(4960 * 1000);
    clock_t start = clock();
    sort(arr, arr + 4960 * 1000);
    clock_t finish = clock();
    ans += 1.0 * (finish - start) / 100;
    }
    cout << ans << endl;
    return 0;
}
