#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        int maximumElementAfterDecrementingAndRearranging(vector<int>& arr) {
            sort(arr.begin(), arr.end());
            int n = arr.size();
            arr[0] = 1;
            for (int i = 1; i < n; ++i){
                if (arr[i] - arr[i-1] > 1){
                    arr[i] = arr[i-1] + 1;
                }
            }
            return arr[n-1];
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