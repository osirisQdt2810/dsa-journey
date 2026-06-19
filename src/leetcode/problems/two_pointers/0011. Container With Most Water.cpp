#include <vector>
#include <iostream>

using namespace std;

class Solution {
    public:
        int maxArea(vector<int>& height) {
            // quan sát: giả sử ta đang đứng tại (h[l], h[r])
            // area = min(h[l], h[r]) * (r-l)
            // nếu h[l] < h[r] => area = h[l] * (r-l) > min(h[l], h[k]) * (r-k) với mọi k chạy từ l đổ đi => tức là area(l, r) đã là tốt nhất trong các đoạn (l, k) với r>k>l
            // => ta có thể ignore luôn case tại l, tăng l lên 1 
            // => ta rút ra kinh nghiệm khi làm việc với two pointer 2 đầu mút.
            // giả sử tại (l, r). Ta có thể tăng được l++ hoặc r-- (giả sử l++, khi và chỉ khi: ta không cần xét tới case (l, k) với k<r nữa
            // KHÔNG CẦN XÉT TỚI xảy ra khi nào?
            //      KHI: (l, r) hiện tại TỐT hơn bất kì (l, k) với k < r nào

            // trong bài toán này: tại (l, r) TỐT hơn mọi (l, k) với k < r <=> min(h[l], h[r])*(r-l) > min(h[l],h[k])*(k-l) <=> h[l] < h[r]
            int n = height.size();
            int l = 0, r = n-1;
            int max_area = 0;
            while (l < r){
                int local_area = min(height[l], height[r]) * (r-l);
                max_area = max(max_area, local_area);
                if (height[l] < height[r]){
                    ++l;
                }
                else {
                    --r;
                }
            }
            return max_area;
            
        }

        int maxAreaSort(vector<int>& height) {
            using ii = pair<int, int>;
            int n = height.size();
            vector<ii> nodes(n);
            for (int i = 0; i < n; ++i) nodes[i] = {height[i], i};
            sort(nodes.begin(), nodes.end(), greater<ii>());

            int max_idx = nodes[0].second, min_idx = nodes[0].second;
            int max_area = -1;
            for (int r = 1; r < n; ++r){
                int hr = nodes[r].first;
                int ir = nodes[r].second;
                int width = max(abs(max_idx - ir), abs(min_idx - ir));
                int larea = width * hr;
                // cout << "h=" << h << " i=" << ir << " w=" << width << " larea=" << larea << "\n";
                max_area = max(max_area, larea);
                max_idx = max(max_idx, ir);
                min_idx = min(min_idx, ir);
            }
            return max_area;
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
    vector<int> height = {1,8,6,2,5,4,8,3,7};

    cout << sol.maxArea(height);

    // h[1] h[2] h[3] h[4] h[5] h[6]
    // (l r) => min(h[l], h[r]) * (r-l)
    return 0;
}