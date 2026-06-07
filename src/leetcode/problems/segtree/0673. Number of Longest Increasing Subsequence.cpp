#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Solution {
    private:
        struct lis {
            int len{0};
            int cnt{0};

            lis operator+(const lis& other){
                if (len == other.len){
                    return {len, cnt + other.cnt};
                }
                return (len > other.len) ? *this : other;
            }
        };

        struct segtree {
            vector<lis> nodes;
            int n;

            segtree(int n_) : n(n_), nodes(4*n_+1){}

            lis query(int t, int l, int r, int X, int Y){
                // case 1: [l...r][X...Y] or [X...Y][l...r]
                if (r < X || Y < l) return {0, 0};
                // case 2: [X...l...r...Y]
                if (X <= l && r <= Y) return nodes[t];
                int m = (l+r)/2;
                return query(2*t,l,m,X,Y) + query(2*t+1,m+1,r,X,Y);
            }

            lis query(int l, int r){
                return query(1,1,n,l,r);
            }

            void update(int t, int l, int r, int k, lis val){
                if (l == r){
                    nodes[t] = val;
                    return;
                }
                int m = (l+r)/2;
                (k <= m) ? update(2*t,l,m,k,val) : update(2*t+1,m+1,r,k,val);
                nodes[t] = nodes[2*t] + nodes[2*t+1];
            }

            void update(int k, lis val){
                update(1,1,n,k,val);
            }

            bool is_null(lis t){
                return t.len == 0;
            }
        };

    public:
        int findNumberOfLIS(vector<int>& nums) {
            int n = nums.size();
            vector<lis> dp(n);  // dp[i]: LIS and its occurence in nums[0...i] (i-included)

            int r = 0;
            map<int, int> coordinate;
            for (auto num : nums) coordinate[num] = 0;
            for (auto& c : coordinate) c.second = ++r;

            segtree tree(r);

            lis ans;

            for (int i = 0; i < n; ++i){
                // find j where j < i and:
                //      1. nums[j] < nums[i]
                //      2. dp[j] max
                // <=> find j where j < coordinate[nums[i]] and dp[j] max
                int id = coordinate[nums[i]];
                lis j = tree.query(1, id-1);
                lis curr = {j.len + 1, (tree.is_null(j) ? 1 : j.cnt)};
                ans = ans + curr;
                lis old = tree.query(id, id);
                tree.update(id, old + curr);
            }
            return ans.cnt;
        }
};

