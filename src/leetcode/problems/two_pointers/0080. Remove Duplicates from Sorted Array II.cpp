#include <vector>
#include <iostream>
#include <unordered_set>

using namespace std;

class Solution {
    public:
        int removeDuplicates(vector<int>& nums) {
            int k = 1, n = nums.size();
            int cnt = 1;
            for (int r = 1; r < n; ++r){
                if (nums[r] == nums[k-1]){
                    if (cnt == 2) continue;
                    ++cnt;
                    nums[k++] = nums[r];
                    // cout << "1. k=" << k << " r=" << r << " num=" << nums[r] << "\n";
                }
                else {  // nums[r] > nums[k]
                    cnt = 1;
                    nums[k++] = nums[r];
                    // cout << "2. k=" << k << " r=" << r << " num=" << nums[r] << "\n";
                }
            }
            return k;
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
    vector<int> nums = {0,0,1,1,1,1,2,3,3};
    int newLength = sol.removeDuplicates(nums);
    for (int i = 0; i < newLength; ++i) {
        cout << nums[i] << " ";
    }
    cout << endl;

    return 0;
}