#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        int maxNumberOfBalloons(string text) {
            unordered_map<char, int> balloons = {
                {'a', 0},
                {'b', 0},
                {'l', 0},
                {'o', 0},
                {'n', 0}
            };
            for (auto& c : text){
                if (balloons.find(c) != balloons.end()){
                    balloons[c]++;
                }
            }
            int res = 1e9;
            res = min(res, balloons['b']);
            res = min(res, balloons['a']);
            res = min(res, balloons['l'] / 2);
            res = min(res, balloons['o'] / 2);
            res = min(res, balloons['n']);
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
    string text = "loonbalxballpoon";
    cout << sol.maxNumberOfBalloons(text) << endl;
    return 0;
}