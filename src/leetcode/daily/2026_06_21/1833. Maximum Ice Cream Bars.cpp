#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        // insertion sort
        // {1,3,2,4,1}
        // 1 2 3 4 0.5
        void sort(vector<int>& costs, int l, int r){
            for (int i = l+1; i < r; ++i){
                int tmp = costs[i];
                int j = i;
                while (j > l && costs[j-1] > tmp){
                    costs[j] = costs[j-1];
                    --j;
                }
                costs[j] = tmp;
            }
            // cout << "l=" << l << ", r=" << r << endl;
            // for (int i = l; i < r; ++i){
            //     cout << costs[i] << " ";
            // }
            // cout << endl;
        }

    public:
        int maxIceCream(vector<int>& costs, int coins) {
            int n = costs.size();
            const int BUCKET_SIZE = 100;
            int BUCKETS = 0;

            struct index {
                int i;
                int maxsz;
            };
            using Bucket = int;
            using Cost = int;
            using cb = pair<Cost, Bucket>;

            unordered_map<Bucket, index> pos;
            priority_queue<cb, vector<cb>, greater<cb>> pq;

            for (int i = 0; i < n; i += BUCKET_SIZE){
                int l = i;
                int r = min(n, i + BUCKET_SIZE);
                sort(costs, l, r);
                pos[BUCKETS++] = {l, r};
            }

            for (auto& [b, idx] : pos){
                pq.push({costs[idx.i], b});
            }

            int res = 0;
            while (!pq.empty()){
                auto [cost, bucket] = pq.top(); pq.pop();
                if (coins < cost) break;
                coins -= cost;
                ++res;
                int pointer = pos[bucket].i;
                // cout << "bucket=" << bucket << ", cost=" << cost << ", coins=" << coins << " - pointer=" << pointer << endl;
                int max_sz = pos[bucket].maxsz;
                if (pointer + 1 < max_sz){
                    pq.push({costs[pointer+1], bucket});
                    pos[bucket].i++;
                }
            }
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
    vector<int> costs = {1,3,2,4,1};
    int coins = 7;
    cout << sol.maxIceCream(costs, coins) << endl;
    return 0;
}