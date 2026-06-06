#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        static const int MAX_N = 1e5+1;
        int waviness[MAX_N];
        int sum[MAX_N];

    public:
        Solution(){
            memset(waviness, 0, sizeof(waviness));
            memset(sum, 0, sizeof(sum));
            for (int n = 100; n < MAX_N; ++n){
                int d3 = (n % 10);
                int d2 = (n / 10) % 10;
                int d1 = (n / 100) % 10;
                int wave = (d2 > max(d1, d3) || d2 < min(d1, d3));
                waviness[n] = waviness[n/10] + wave;
                sum[n] = sum[n-1] + waviness[n];
            }
        }

        int totalWaviness(int num1, int num2) {
            return sum[num2] - sum[num1-1];
        }

        int totalWaviness2(int num1, int num2) {
            auto waviness = [](int n) -> int {
                vector<int> digits;
                while (n){
                    digits.push_back(n % 10);
                    n /= 10;
                }
                if (digits.size() < 3) return 0;
                int _waviness = 0;
                for (int ptr = 1; ptr < (int)digits.size() - 1; ++ptr){
                    _waviness += (digits[ptr] > digits[ptr-1] && digits[ptr] > digits[ptr+1]);
                    _waviness += (digits[ptr] < digits[ptr-1] && digits[ptr] < digits[ptr+1]);
                }
                return _waviness;
            };
            int ans = 0;
            for (int n = num1; n <= num2; ++n)
                ans += waviness(n);
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

    int num1=4848, num2=4848;
    cout << sol.totalWaviness(num1, num2) << endl;

    return 0;
}