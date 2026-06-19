#include <vector>
#include <iostream>

using namespace std;

class Solution {
    public:
        void sortColors(vector<int>& nums) {
            // [2,0,2,1,1,0]
            // output: [0,0,0,0,1,1]
            int n = nums.size();
            int cnt_0 = 0, cnt_1 = 0, cnt_2 = 0;
            for (int& num : nums){
                if (num == 0) cnt_0++;
                if (num == 1) cnt_1++;
                if (num == 2) cnt_2++;
            }
            cnt_0;
            cnt_1 += cnt_0;
            cnt_2 += cnt_1;
            int i = 0;
            for (; i < cnt_0; ++i) nums[i] = 0;
            for (; i < cnt_1; ++i) nums[i] = 1;
            for (; i < cnt_2; ++i) nums[i] = 2;
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