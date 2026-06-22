#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

// Main point: two-pass constraint propagation (greedy left->right then right->left)
// to relax restrictions, then find the peak between adjacent constrained points.
class Solution {
    public:
        int maxBuilding(int n, vector<vector<int>>& restrictions) {
            // ban đầu, ta mặc định các building sẽ đạt max height là restrictions.
            // duyệt từ trái sang phải, kiểm tra xem với max height hiện tại liệu đã đạt chưa?
            // vd: 1    2   3   4   5
            //     0    1       5   
            // rõ ràng 2 và 4 chưa đạt, max[4] = min(max[4], max[2] + (4-2))
            restrictions.push_back({1, 0});
            sort(restrictions.begin(), restrictions.end(), [](const vector<int>& lhs, const vector<int>& rhs){
                return lhs[0] < rhs[0];
            });
            int R = restrictions.size();
            for (int i = 1; i < R; ++i){
                int x1 = restrictions[i-1][0], h1 = restrictions[i-1][1];
                int x2 = restrictions[i][0], h2 = restrictions[i][1];
                restrictions[i][1] = min(h2, h1 + (x2 - x1));
            }
            // duyệt lại 1 lượt từ phải sang trái, bởi vì ở lượt trước ta chỉ đang xét: restrictions[i][1] = min(restrictions[i][1], restrictions[i-1][1] + (x2 - x1));
            // sẽ như thế nào nếu bản thân restrictions[i][1] đã rất nhỏ. VD: [2, 5], [3, 1] => chỉ cập nhật left->right, [3,1] giữ nguyên
            for (int i = R-2; i >= 0; --i){
                int x1 = restrictions[i][0], h1 = restrictions[i][1];
                int x2 = restrictions[i+1][0], h2 = restrictions[i+1][1];
                restrictions[i][1] = min(h1, h2 + (x2 - x1));
            }

            // oke, hiện tại sau 2 vòng lặp, ta đã tìm được 1 bộ heights thoả mãn toàn bộ restrictions.
            // bây giờ ta cần tìm max heights dựa trên bộ restrictions hiện tại này
            // để ý, với 1 cặp liên tiếp: (x1, h1) và (x2, h2) thì ta có thể tìm được x_H, x1 <= x_H <= x2 mà H(x_H) là peak
            // cụ thể H = floor((h1 + h2 + x2 - x1) / 2)
            int res = -1;
            for (int i = 1; i < R; ++i){
                int x1 = restrictions[i-1][0], h1 = restrictions[i-1][1];
                int x2 = restrictions[i][0], h2 = restrictions[i][1];
                res = max(res, (int)floor((h1 + h2 + x2 - x1) / 2));
            }

            // đoạn cuối từ toà nhà cuối cùng tới n
            int x_last = restrictions[R-1][0], h_last = restrictions[R-1][1];
            res = max(res, h_last + n - x_last);
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