#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Solution {
private:
    struct segtree {
        int n;
        vector<long long > nodes;

        segtree(int n_) : n(n_), nodes(4*n_+1){}

        long long query(int t, int l, int r, int X, int Y){
            // case 1: [l...r][X...Y] or [X...Y][l...r]
            if (r < X || Y < l) return 0;
            // case 2: [X...l...r...Y]
            if (X <= l && r <= Y) return nodes[t];
            // case 3: overlap
            int m = (l+r)/2;
            return query(2*t,l,m,X,Y)+query(2*t+1,m+1,r,X,Y);
        }

        long long query(int l, int r){
            if (l > r || l < 0) return 0;
            return query(1,1,n,l+1,r+1);
        }

        void inc(int t, int l, int r, int pos){
            // case 1: [l...r][pos] or [pos][l...r]
            if (r < pos || pos < l) return;
            // case 2: [pos...l...r...pos]
            if (l == r){
                nodes[t]++;
                return;
            }
            // case 3: overlap
            int m = (l+r)/2;
            inc(2*t,l,m,pos);
            inc(2*t+1,m+1,r,pos);
            nodes[t]=nodes[2*t]+nodes[2*t+1];
        }
        void inc(int id){
            inc(1,1,n,id+1);
        }
    };

public:
    long long numberOfPairs(vector<int>& nums1, vector<int>& nums2, int diff) {
        int n = nums1.size();
        for (int i = 0; i < n; ++i){
            nums1[i] = nums1[i] - nums2[i];
            nums2[i] = nums1[i];
        }
        sort(nums2.begin(), nums2.end());
        nums2.erase(unique(nums2.begin(), nums2.end()), nums2.end());

        int un = nums2.size();
        segtree tree(un);

        // first index id where nums2[id] <= target
        auto find_id = [&](int target){
            int l = 0, r = un-1, id=-1;
            while (l <= r){
                int m = (l+r)/2;
                if (nums2[m] <= target){
                    id=m;
                    l=m+1;
                }
                else {
                    r=m-1;
                }
            }
            return id;
        };

        long long ans=0;
        for (int i = 0; i < n; ++i){
            int id = find_id(nums1[i] + diff);
            ans += tree.query(0, id);
            int id2 = find_id(nums1[i]);
            tree.inc(id2);
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
    vector<int> nums1 = {3,2,5}, nums2 = {2,2,1}; // 1, 0, 4
    int diff = 1;
    Solution s;
    cout << s.numberOfPairs(nums1, nums2, diff) << "\n";
    return 0;
}