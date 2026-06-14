#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
private:
    int add(int a, int b){
        int c = (a + b);
        return (c < 26) ? c : (c - 26);
    }

public:
    string mapWordWeights(vector<string>& words, vector<int>& weights) {
        string res = "";
        for (auto word : words){
            int s = 0;
            for (int c : word){
                s += weights[c-'a'];
            }
            s %= 26;
            res += char('z' - s);
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