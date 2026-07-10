#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        vector<int> pathExistenceQueries(int n, vector<int>& nums, int maxDiff, vector<vector<int>>& queries) {
            vector<int> res;
            if (n == 1){
                for (auto& q : queries) res.push_back(0);
                return res;
            }

            using ii = pair<int, int>;
            vector<ii> nums_with_index; 
            for (int i = 0; i < n; ++i) nums_with_index.push_back({nums[i], i}); 
            sort(nums_with_index.begin(), nums_with_index.end());

            for (int i = 0; i < n; ++i) nums[nums_with_index[i].second] = i;
            
            const int LOG = ceil(log2(n));
            const int INF = -1;

            vector<vector<int>> st(n, vector<int>(LOG, INF));        // st[i][0]: đỉnh k0=st[i][0] xa nhất so với i mà nums[k0] - nums[i] <= maxDiff
                                                                    // st[i][1]: đỉnh k1=st[k0] xa nhất so với k0 mà nums[k1] - nums[k0] <= maxDiff....
            for (int l = 0, r = 0; l < n; ++l){
                while (r < n && nums_with_index[r].first - nums_with_index[l].first <= maxDiff){
                    r++;
                }
                int fr = r-1;
                st[l][0] = (fr < n && fr > l) ? fr : INF;
            }

            for (int j = 1; j < LOG; ++j){
                for (int i = 0; i < n; ++i){
                    if (st[i][j-1] != INF){
                        st[i][j] = st[st[i][j-1]][j-1];
                    }
                }
            }

            // for (int j = 0; j < LOG; ++j){
            //     for (int i = 0; i < n; ++i){
            //         cout << st[i][j] << " ";
            //     }
            //     cout << "\n";
            // }

            for (auto& query : queries){
                int q0 = nums[query[0]];
                int q1 = nums[query[1]];
                int l = min(q0, q1);
                int r = max(q0, q1);
                if (l == r){
                    res.push_back(0);
                    continue;
                }
                
                int step = 0;
                // cout << "l=" << l << " r=" << r << " st[l][0]=" << st[l][0] << "\n";
                for (int j = LOG-1; j >= 0; --j){
                    if (st[l][j] != INF && st[l][j] < r){
                        // cout << "\tl=" << l << " r=" << r << " j=" << j << " st=" << step << "\n";
                        l = st[l][j];
                        step += (1 << j);
                    }
                }
                res.push_back((st[l][0] >= r) ? (step + 1) : -1);
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

    vector<int> nums = {1,8,3,4,2};
    int maxDiff = 3;
    int n = 5;
    vector<vector<int>> queries = {{0,3}, {2,4}};
    
    auto res = sol.pathExistenceQueries(n, nums, maxDiff, queries);
    cout << "\n"; for (auto r : res) cout << r << " ";
    return 0;
}