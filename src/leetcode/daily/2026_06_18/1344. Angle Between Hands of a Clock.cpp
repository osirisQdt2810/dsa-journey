#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        double angleClock(int hour, int minutes) {
            hour = (hour < 12) ? hour : (hour - 12);
            double minute_angle = (1.0 * minutes / 60) * 360;
            double hour_angle = ((1.0 * hour + 1.0 * minutes / 60) / 12) * 360;
            double angle = max(minute_angle, hour_angle) - min(minute_angle, hour_angle);
            return (angle < 180) ? angle : (360 - angle);
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