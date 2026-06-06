#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
public:
    vector<int> leftRightDifference(vector<int>& nums) {
        int sum = 0, n = nums.size();
        for (int i = 0; i < n; ++i) sum += nums[i];
        for (int left_sum=0, i = 0; i < n; ++i){
            int tmp = left_sum + nums[i];
            nums[i] = abs(sum - nums[i] - 2 * left_sum);
            left_sum = tmp;
        }
        return nums;
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