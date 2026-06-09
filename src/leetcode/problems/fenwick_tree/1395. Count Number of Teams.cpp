#include <vector>
#include <iostream>
#include <set>

using namespace std;

class Solution {
    private:
        struct fenwicktree {
            int n;
            vector<int> bits;

            fenwicktree(int n_) : n(n_), bits(n_+1){}

            int _query(int pos){    // [1...pos]
                int res = 0;
                while (pos){
                    res += bits[pos];
                    pos -= (pos & (-pos));
                }
                return res;
            }

            void _inc(int pos, int delta){
                // find all X where bits[X] contains pos
                while (pos <= n){
                    bits[pos] += delta;
                    pos += (pos & (-pos));
                }
            }

            int query(int idx){
                return _query(idx+1);
            }

            void inc(int idx, int delta){
                return _inc(idx+1, delta);
            }
        };

    public:
        int numTeams(vector<int>& rating) {
            return numTeamsFenwickTree(rating);
        }

        int numTeamsFenwickTree(vector<int>& rating) {
            int n = rating.size();
            vector<int> sorted_ratings(rating);
            sort(sorted_ratings.begin(), sorted_ratings.end());
            unordered_map<int, int> rating2idx;
            for (int i = 0; i < n; ++i) rating2idx[sorted_ratings[i]] = i;

            vector<int> left(n, 0);
            fenwicktree ltree(n);
            for (int i = 0; i < n; ++i){
                // find number of elements T where T > rating[i]
                int idx = rating2idx[rating[i]];
                left[i] = ltree.query(idx);
                ltree.inc(idx, 1);
            }

            vector<int> right(n, 0);
            fenwicktree rtree(n);
            int teams = 0;
            for (int i = n-1; i >= 0; --i){
                // find number of elements T where T > rating[i]
                int idx = rating2idx[rating[i]];
                right[i] = rtree.query(idx);
                rtree.inc(idx, 1);
            }

            for (int i = 0; i < n; ++i){
                cout << "i=" << i << ", rating=" << rating[i] << ", left=" << left[i] << ", right=" << right[i] << endl;
                teams += (left[i]) * (n-i-1-right[i]);
                teams += (i-left[i]) * right[i];
            }
            
            return teams;
        }

        int numTeamsBruteForce(vector<int>& rating) {
            int n = rating.size();
            vector<int> greater(n, 0);
            vector<int> less(n, 0);
            for (int i = 0; i < n; ++i){
                greater[i] = 0;
                less[i] = 0;
                for (int j = i+1; j < n; ++j){
                    greater[i] += (rating[j] > rating[i]);
                    less[i] += (rating[j] < rating[i]);
                }
            }
            
            int teams = 0;
            for (int i = 0; i < n; ++i){
                for (int j = i+1; j < n; ++j){
                    if (rating[i] < rating[j]) teams += greater[j];
                    if (rating[i] > rating[j]) teams += less[j];
                }
            }
            return teams;
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