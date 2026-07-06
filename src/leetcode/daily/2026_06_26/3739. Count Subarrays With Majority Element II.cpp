#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        using ll = long long;
        struct fenwick {
            int n;
            vector<int> bits;   // bits[k]: sum(x[k-2^t]...x[k]), trong đó t: least meaning bit, 2^t = (k&(-k))

            fenwick(int n_) : n(n_), bits(n+1, 0){}

            int sum(int v){ // find sum[x[1]...v]
                int res = 0;
                while (v){
                    res += bits[v];
                    v -= v & (-v);
                }
                return res;
            }

            void add(int k, int val){   // x[k] += val
                while (k <= n){
                    bits[k] += val;
                    k += k & (-k);
                }
            }
        };

    public:
        long long countMajoritySubarrays(vector<int>& nums, int target) {
            int n = nums.size();
            int counter = 0;
            vector<int> pref(n+1, 0);
            // for (int r = 0; r < n; ++r){
            //     pref[r+1] = pref[r] + (nums[r] == target);
            // }
            // for (int r = 0; r < n; ++r){
            //     pref[r+1] = (2 * pref[r+1] - (r+1)) + n;
            // }
            const int offset = n+2;
            fenwick tree(2*n+3);
            tree.add(0 + offset, 1);

            long long res = 0;
            for (int prev=0, curr=0, r=0; r < n; ++r){
                // find number of l where counter([l...r]) > (r-l+1)/2
                // find number of l where 2 * (pref[r+1] - pref[l]) > (r-l+1)
                //                  <=>   2 * (pref[r+1] - (r+1)) > 2 * pref[l] - l
                //                  <=>   pref'[r+1] > pref'[l]
                curr = prev + (nums[r] == target);
                int val = 2 * curr - (r + 1);
                int idx = val + offset;
                res += tree.sum(idx-1);
                tree.add(idx, 1);
                prev = curr;
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

    vector<int> nums = {1,2,2,3};
    int target = 2;
    long long result = sol.countMajoritySubarrays(nums, target);
    cout << "Number of majority subarrays: " << result << endl;
    return 0;
}