#include <iostream>
#include <vector>

using namespace std;

class Solution {
    public:
        vector<int> corpFlightBookings(vector<vector<int>>& bookings, int n) {
            vector<int> d(n+1, 0);
            for (auto& booking : bookings){
                int f = booking[0], l = booking[1], s = booking[2];
                d[f] += s;
                d[l+1] -= s;
            }
            vector<int> res;
            for (int i=1; i <= n; ++i){
                d[i] += d[i-1];
                res.push_back(d[i]);
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