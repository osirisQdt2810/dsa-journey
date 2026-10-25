#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
using namespace std;

using ll = long long;
using ii = pair<int, int>;

class Solution {
    private:
        vector<int> compress(vector<int>& nums){
            vector<int> vals = nums;
            sort(vals.begin(), vals.end());
            vals.erase(unique(vals.begin(), vals.end()), vals.end());
            return vals;
        }
    
    public:
        int countStaircase(vector<int>& nums) {
            int n = nums.size();
            vector<int> vals = compress(nums);

            // auto rank = [&](int X){
            //     return lower_bound(vals.begin(), vals.end(), X) - vals.begin();
            // };

            vector<int> rank(n);
            for (int i = 0; i < n; ++i){
                rank[i] = lower_bound(vals.begin(), vals.end(), nums[i]) - vals.begin();
            }
            

            int m = vals.size();
            vector<int> freq(m, 0), target(m, 0);
            vector<int> visited;

            int ans = 0;
            for (int i = 0; i < n; ++i){
                int k = 0;
                for (int j = i; j < n; ++j){
                    int v = rank[j];
                    if (target[v] == 0){
                        ++k;
                        target[v] = k;
                        freq[v] = 1;
                        visited.push_back(v);
                    }
                    else {
                        if (++freq[v] > target[v]){
                            break;
                        }
                    }
                    long long ss = 1LL * (j-i+1);
                    long long se = 1LL * k * (k+1) / 2;
                    ans += ss == se;
                }

                for (int v : visited){
                    target[v] = 0;
                    freq[v] = 0;
                }
                visited.clear();
            }
            return ans;
        }
};

int main(){
    Solution sol;

    vector<int> nums = {5,8,8,2,2,2};

    cout << sol.countStaircase(nums);
    return 0;
}