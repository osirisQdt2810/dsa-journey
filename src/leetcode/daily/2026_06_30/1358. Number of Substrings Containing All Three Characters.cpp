#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
public:
    int numberOfSubstrings(string s) {
        int n = s.size();
        int counter_a = 0, counter_b = 0, counter_c = 0;
        
        auto inc = [&](char x){
            if (x == 'a') counter_a++;
            else if (x == 'b') counter_b++;
            else if (x == 'c') counter_c++;
        };

        auto dec = [&](char x){
            if (x == 'a') counter_a--;
            else if (x == 'b') counter_b--;
            else if (x == 'c') counter_c--;
        };

        int ans = 0;

        for (int l = 0, r = 0; r < n; ++r){
            inc(s[r]);
            while (
                counter_a > 0 &&
                counter_b > 0 &&
                counter_c > 0
            ){
                dec(s[l]);
                ++l;
            }
            ans += l;
        }
        return ans;
    }

    int numberOfSubstringsMap(string s) {
        int n = s.size();
        unordered_map<char, int> counter;
        int ans = 0;
        for (int l = 0, r = 0; r < n; ++r){
            char c = s[r];
            counter[c]++;
            while (
                counter['a'] > 0 &&
                counter['b'] > 0 &&
                counter['c'] > 0
            ){
                counter[s[l++]]--;
            }
            ans += l;
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

    return 0;
}