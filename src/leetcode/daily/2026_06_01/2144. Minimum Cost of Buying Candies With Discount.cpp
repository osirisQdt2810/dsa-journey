#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
public:
    int minimumCost(vector<int>& cost) {
        int n = cost.size();
        if (n < 2) return cost[0];

        sort(cost.begin(), cost.end(), [](const int& lhs, const int& rhs){ return lhs > rhs; });

        int ans = 0;
        for (int i = 0; i < n; i += 3){
            ans += cost[i] + ((i+1<n) ? cost[i+1] : 0);
        }
        return ans;
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

    vector<int> cost = {3,3,3,1};
    // vector<int> cost = {6,5,7,9,2,2};
    // vector<int> cost = {6,5,7,9,2,2};
    int result = sol.minimumCost(cost);
    cout << result << endl;
    return 0;
}