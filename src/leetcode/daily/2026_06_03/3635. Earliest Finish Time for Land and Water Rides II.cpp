#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
private:
    int solve_A2B(vector<int>& As, vector<int>& Ad, vector<int>& Bs, vector<int>& Bd){
        int m = As.size(), n = Bs.size();
        int ans = INT_MAX, Aend = INT_MAX;
        for (int i = 0; i < m; ++i){
            Aend = min(Aend, As[i] + Ad[i]);
        }
        for (int i = 0; i < n; ++i){
            ans = min(ans, max(Bs[i], Aend) + Bd[i]);
        }
        return ans;
    }

public:
    int earliestFinishTime(vector<int>& landStartTime, vector<int>& landDuration, vector<int>& waterStartTime, vector<int>& waterDuration) {
        return min(
            solve_A2B(landStartTime, landDuration, waterStartTime, waterDuration),
            solve_A2B(waterStartTime, waterDuration, landStartTime, landDuration)
        );
    }
    int earliestFinishTime2(vector<int>& landStartTime, vector<int>& landDuration, vector<int>& waterStartTime, vector<int>& waterDuration) {
        int n = landStartTime.size();
        int m = waterStartTime.size();

        int ans = INT_MAX;
        for (int i = 0; i < m; ++i){
            for (int t, j = 0; j < n; ++j){
                int land_start  = landStartTime[j];
                int land_end    = land_start + landDuration[j];
                int water_start = waterStartTime[i];
                int water_end   = water_start + waterDuration[i];
                // [X, X+dur]
                // [Y, Y+dur]
                if (land_end <= water_start){
                    t = water_end;
                }
                else if (water_end <= land_start){
                    t = land_end;
                }
                else {
                    t = min(land_end + waterDuration[i], water_end + landDuration[j]);
                }
                ans = min(ans, t);
            }
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