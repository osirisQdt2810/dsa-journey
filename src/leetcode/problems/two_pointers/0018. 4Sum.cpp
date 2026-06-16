#include <vector>
#include <iostream>
#include <unordered_set>

using namespace std;

class Solution {
    private:
        string to_string(vector<int>& lst){
            return std::to_string(lst[0]) + "|" + std::to_string(lst[1]) + "|" + std::to_string(lst[2]) + "|" + std::to_string(lst[3]);
        }

    public:
        vector<vector<int>> fourSum2(vector<int>& nums, int target) {
            int n = nums.size();
            vector<vector<int>> res;

            unordered_set<string> visited;  // to avoid duplicate quadruplets

            sort(nums.begin(), nums.end());
            long long ll_target = (long long)(target);
            for (int i = 0; i < n; ++i){
                for (int j = i+1; j < n; ++j){
                    // find k, l such that: nums[k] + nums[l] == target - (nums[i] + nums[j])
                    int l = j+1, r=n-1;
                    long long tij = ll_target - nums[i] - nums[j];
                    while (l < r){
                        long long sumlr = nums[l] + nums[r];
                        if (sumlr < tij){   // increase nums[l]
                            ++l;
                        }
                        else if (sumlr > tij){  // decrease nums[r]
                            --r;
                        }
                        else {  // (l, r) is a satisfying pair
                            vector<int> ans = {nums[i], nums[j], nums[l], nums[r]};
                            string hash = to_string(ans);
                            if (!visited.count(hash)){
                                res.emplace_back(std::move(ans));
                                visited.insert(hash);
                            }
                            ++l; --r;
                            while (l < r && nums[l] == nums[l-1]) ++l;
                            while (l < r && nums[r] == nums[r+1]) --r;
                        }
                    }
                }
            }
            return res;
        }

        vector<vector<int>> fourSum(vector<int>& nums, int target) {
            int n = nums.size();
            vector<vector<int>> res;

            unordered_set<string> visited;  // to avoid duplicate quadruplets

            sort(nums.begin(), nums.end());
            long long ll_target = (long long)(target);
            for (int i = 0; i < n; ++i){
                for (int j = i+1; j < n; ++j){
                    // find k, l such that: nums[k] + nums[l] == target - (nums[i] + nums[j])
                    int l = j+1, r=n-1;
                    long long tij = ll_target - nums[i] - nums[j];
                    while (l < r){
                        long long sumlr = nums[l] + nums[r];
                        if (sumlr < tij){   // increase nums[l]
                            ++l;
                        }
                        else if (sumlr > tij){  // decrease nums[r]
                            --r;
                        }
                        else {  // (l, r) is a satisfying pair
                            vector<int> ans = {nums[i], nums[j], nums[l], nums[r]};
                            res.emplace_back(std::move(ans));
                            ++l; --r;
                            while (l < r && nums[l] == nums[l-1]) ++l;
                            while (l < r && nums[r] == nums[r+1]) --r;
                        }
                    }
                    while (j+1<n && nums[j] == nums[j+1]) j++;
                }
                while (i+1<n && nums[i] == nums[i+1]) i++;
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
    vector<int> nums = {1,0,-1,0,-2,2};
    nums = {2,2,2,2,2};
    int target = 8;

    vector<vector<int>> result = sol.fourSum(nums, target);
    for (const auto& quad : result) {
        for (int num : quad) {
            cout << num << " ";
        }
        cout << endl;
    }

    return 0;
}