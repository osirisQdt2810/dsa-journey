#include <vector>
#include <iostream>
#include <unordered_set>

using namespace std;

class Solution {
    public:
        int trap(vector<int>& height) {
            // {0,1,0,2,1,0,1,3,2,1,2,1};
            int n = height.size();
            int l = 0, r = n-1;
            int lmax = INT_MIN, rmax = INT_MIN;
            int water = 0;

            while (l < r){
                lmax = max(height[l], lmax);
                rmax = max(height[r], rmax);
                if (lmax < rmax){   // thus, at i: min(max_left[i], max_right[i]) = max_left[i] = lmax (this happens even when we dont know the full max_right)
                    water += (lmax - height[l]);
                    l++;
                }
                else {
                    water += (rmax - height[r]);
                    r--;
                }
            }
            return water;
        }
        int trapMonostack(vector<int>& height) {
            int n = height.size();
            vector<int> left_nearest_ge(n, -1);
            vector<int> right_nearest_ge(n, -1);
            stack<int> left_st;
            for (int i = 0; i < n; ++i){
                left_nearest_ge[i] = i;
                while (!left_st.empty() && height[left_st.top()] <= height[i]) left_st.pop();
                if (!left_st.empty()) left_nearest_ge[i] = left_st.top();
                left_st.push(i);
            }
            stack<int> right_st;
            for (int i = n-1; i >= 0; --i){
                right_nearest_ge[i] = i;
                while (!right_st.empty() && height[right_st.top()] <= height[i]) right_st.pop();
                if (!right_st.empty()) right_nearest_ge[i] = right_st.top();
                right_st.push(i);
            }
            unordered_set<int> calculated;
            int trapped_water = 0;
            for (int i = 0; i < n; ++i){
                int l = left_nearest_ge[i];
                int r = right_nearest_ge[i];
                if (i == l || i == r) continue;
                int hash = l * n + r;
                // cout << "i=" << i << " l=" << l << " r=" << r << "\n";
                if (!calculated.count(hash)){
                    calculated.insert(hash);
                    int water = (min(height[l], height[r]) - height[i]) * (r-l-1);
                    trapped_water += water;
                    // cout << "\t-> water=" << water << "\n";
                }
                else {
                    // cout << "\t-> water=" << 0 << " because existed\n";
                }
            }
            return trapped_water;
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
    vector<int> height = {0,1,0,2,1,0,1,3,2,1,2,1};
    int trapped_water = sol.trap(height);
    cout << "Trapped water: " << trapped_water << endl;
    return 0;
}