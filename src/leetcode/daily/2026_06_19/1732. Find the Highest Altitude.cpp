#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        int largestAltitude(vector<int>& gain) {
            int n = gain.size();
            int d = gain[0], res = max(d, 0);
            for (int i = 1; i < n; ++i){
                d += gain[i];
                res = max(res, d);
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