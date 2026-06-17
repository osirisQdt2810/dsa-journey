#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        char processStr(string s, long long k) {
            int n = s.size();
            long long len = 0;
            for (auto& c : s){
                if (c >= 'a' && c <= 'z') ++len;
                if (c == '*') len = max(len-1, 0LL);
                if (c == '#') len *= 2;
            }
            if (k >= len) return '.';
            // cout << "before: len=" << len << " k=" << k << "\n";
            for (int i = n-1; i >= 0; --i){
                char c = s[i];
                if (c >= 'a' && c <= 'z'){
                    len = max(len-1, 0LL);
                    if (k == len) return c;
                }
                else if (c == '*'){
                    ++len;
                }
                else if (c == '#'){
                    len /= 2;
                    k = (k >= len) ? (k-len) : k;
                }
                else {
                    k = len - k - 1;
                }
                // cout << "\t after: c=" << c << " len=" << len << " k=" << k << "\n";
            }
            // cout << "after: len=" << len << " k=" << k << "\n";
            return s[k];
        }

    string genStr(string s) {
        string res = "";
        for (auto& c : s){
            if ('a' <= c && c <= 'z'){
                res += c;
            }
            else if (c == '*'){
                if (!res.empty()) res.pop_back();
            }
            else if (c == '#'){
                res += res;
            }
            else if (c == '%'){
                reverse(res.begin(), res.end());
            }
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
    string s = "a#b%*"; long long k = 1;
    s = "cd%#*#"; k = 3;
    // s = "%#%jv#%"; k = 1;
    cout << sol.genStr(s) << "\n";
    char r = sol.processStr(s, k);
    cout << "\nres=" << r << "\n";
    return 0;
}