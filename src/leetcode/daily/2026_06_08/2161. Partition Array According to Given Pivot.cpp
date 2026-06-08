#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
public:
    vector<int> pivotArray(vector<int>& nums, int pivot) {
        int n = nums.size();
        int l = 0, r = n-1;
        vector<int> res(n);
        int cl=0, cr=n-1;
        for (int l = 0; l < n; ++l){
            if (nums[l] < pivot){
                res[cl++] = nums[l];
            }
        }
        for (int r = n-1; r >= 0; --r){
            if (nums[r] > pivot){
                res[cr--] = nums[r];
            }
        }
        for (int i=cl; i<=cr; ++i){
            res[i] = pivot;
        }
        return res;
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