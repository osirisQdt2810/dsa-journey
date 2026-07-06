#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        int removeCoveredIntervals(vector<vector<int>>& intervals) {
            sort(intervals.begin(), intervals.end(), [](const vector<int>& lhs, const vector<int>& rhs){
                if (lhs[0] < rhs[0]) return true;
                return (lhs[0] == rhs[0] && lhs[1] >= rhs[1]);
            });

            int n = intervals.size();
            int res = 0;
            for (int l = 0; l < n; ){
                int r = l + 1;
                ++res;
                while (r < n && intervals[l][1] >= intervals[r][1]) ++r;
                l = r;
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

    vector<vector<int>> intervals = {{1,4},{3,6},{2,8}};
    cout << sol.removeCoveredIntervals(intervals);

    return 0;
}