#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        long long sumAndMultiply(int n) {
            int new_number = 0;
            int sum = 0;
            int pow = 1;

            while (n){
                int m = n % 10;
                n /= 10;
                if (m){
                    new_number += pow * m;
                    sum += m;
                    pow *= 10; 
                }
            }
            return 1LL * sum * new_number;
        }
};

auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    Solution sol;

    vector<vector<int>> intervals = {{1,4},{3,6},{2,8}};
    cout << sol.removeCoveredIntervals(intervals);

    return 0;
}