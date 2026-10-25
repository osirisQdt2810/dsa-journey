#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <unordered_set>
using namespace std;

class Solution {
    public:
        long long maximizeSum(vector<int>& nums, int k) {
            int n = nums.size();
            using ll = long long;
            vector<ll> diff;
            ll total = 0;
            for (auto num : nums){
                string s_num = to_string(num);
                sort(s_num.rbegin(), s_num.rend());
                diff.push_back(atoll(s_num.c_str()) - num);
                total += num;
            }

            sort(diff.rbegin(), diff.rend());

            for (int i = 0; i < k; ++i){
                total += diff[i];
            }
            return total;
        }
};

int main(){
    Solution sol;

    vector<int> nums = {123,45};
    int k = 1;

    cout << sol.maximizeSum(nums, k);

    return 0;
}

