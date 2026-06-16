#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
public:
    string processStr(string s) {
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
    return 0;
}