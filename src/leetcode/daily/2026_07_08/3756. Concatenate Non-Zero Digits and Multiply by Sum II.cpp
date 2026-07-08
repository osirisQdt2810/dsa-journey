#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        static inline constexpr int mod = 1e9 + 7;
        using ll = long long;

        ll pow(int x, int m){
            if (m == 0) return 1;
            if (m == 1) return x % mod;
            ll r = pow(x, m/2);
            r = (r * r) % mod;
            return (m%2) ? (r * x) % mod : r;
        }

    public:
        vector<int> sumAndMultiply(string s, vector<vector<int>>& queries) {
            int n = s.size();
            vector<int> num(n+1, 0);    // num[i]: số chữ số khác 0 trong khoảng s[0...i-1]
                                        // số chữ số khác 0 trong khoảng s[i...j] = num[j+1] - num[i]
            vector<int> x(n+1, 0);      // x[i]: x được tạo trong khoảng s[0...i-1]
            vector<int> sum(n+1, 0);    // sum[i]: sum các chữ số trong khoảng s[0...i-1]

            for (int i = 0; i < n; ++i){
                int d = s[i] - '0';
                int z = (d!=0);
                sum[i+1] = sum[i] + d;
                num[i+1] = num[i] + z;
                x[i+1] = x[i] * pow(10, z) % mod + d;
            }

            vector<int> res;
            for (auto& query : queries){
                int l = query[0], r = query[1];
                int s = sum[r+1] - sum[l];
                int n = num[r+1] - num[l] ;   // bao nhiêu số trong [l...r]
                ll X = (x[r+1] - x[l] * pow(10, n) % mod + mod) % mod;
                res.push_back(X * s % mod);
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