#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        int countMajoritySubarrays(vector<int>& nums, int target) {
            int n = nums.size();
            vector<int> counter(n+1, 0);
            for (int i = 0; i < n; ++i){
                counter[i+1] = counter[i] + (nums[i] == target);
            }

            int subarrays = 0;
            for (int i = 0; i < n; ++i){
                for (int j = i; j < n; ++j){
                    int target_counter = counter[j+1] - counter[i];
                    subarrays += (target_counter > (j - i + 1) / 2);
                    // cout << "i=" << i << " j=" << j << " half_len=" << (j - i + 1) / 2 << " cnt=" << target_counter << "\n";
                }
            }
            return subarrays;
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