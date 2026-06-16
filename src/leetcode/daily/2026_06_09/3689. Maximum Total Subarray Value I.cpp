#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        long long maxTotalValue(vector<int>& nums, int k) {
            int max = INT_MIN, min = INT_MAX;
            for (auto num : nums){
                if (num > max){
                    max = num;
                }
                if (num < min){
                    min = num;
                }
            }
            return (long long)k * (max - min);
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

    return 0;
}