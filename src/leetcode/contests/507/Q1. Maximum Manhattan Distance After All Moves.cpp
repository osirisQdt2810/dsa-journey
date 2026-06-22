#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

class Solution {
public:
    int maxDistance(string moves) {
        int L=0, R=0, U=0, D=0, UNDERSCORE=0;
        for (char c : moves){
            if (c == 'L') ++L;
            else if (c == 'R') ++R;
            else if (c == 'U') ++U;
            else if (c == 'D') ++D;
            else ++UNDERSCORE;
        }
        return abs(L - R) + abs(U - D) + UNDERSCORE;
    }
};

int main() {
    Solution sol;
    string moves = "RUL";
    cout << sol.maxDistance(moves) << endl;
    return 0;
}