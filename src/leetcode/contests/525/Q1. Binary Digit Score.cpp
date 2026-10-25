#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

class Solution {
    public:
        int calculateScore(int a, int b) {
            auto get_bits = [](int X) -> int {
                return __builtin_popcount(X);
            };

            auto abits = get_bits(a);
            auto bbits = get_bits(b);

            return (abits >= bbits) ? (abits * bbits) : (bbits / abits);
        }
};

int main() {
    Solution sol;
    int  a = 14, b = 6;
    a = 5, b = 15;
    cout << sol.calculateScore(a,b) << endl;
    return 0;
}