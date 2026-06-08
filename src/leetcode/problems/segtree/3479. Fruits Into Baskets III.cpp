#include <iostream>
#include <vector>
#include <map>
#include <deque>

using namespace std;

class Solution {
    private:
        struct segtree {
            int n;
            vector<int> nodes;
            void build(int t, int l, int r, vector<int>& arr){
                if (l == r){
                    nodes[t] = arr[l-1];
                    return;
                }
                int m = (l+r)/2;
                build(2*t,l,m,arr);
                build(2*t+1,m+1,r,arr);
                nodes[t] = max(nodes[2*t], nodes[2*t+1]);
            }

            segtree(vector<int>& baskets) : n(baskets.size()), nodes(4*n+1, 0){
                build(1,1,n,baskets);
            }

            void update(int t, int l, int r, int pos, int val){
                // case 1: [l...r]pos or pos[l...r]
                if (r < pos || pos < l) return;
                // case 2: [pos...l...r...pos]
                if (l == r){
                    nodes[t] = val;
                    return;
                }
                // case 3: overlap
                int m = (l+r)/2;
                update(2*t,l,m,pos,val);
                update(2*t+1,m+1,r,pos,val);
                nodes[t] = max(nodes[2*t], nodes[2*t+1]);
            }

            void update(int pos, int val){
                update(1,1,n,pos,val);
            }

            int query(int t, int l, int r, int val){
                if (nodes[t] < val) return -1;
                if (l == r) return l;
                int m = (l+r)/2;
                int left = query(2*t,l,m,val);
                return (left < 0) ? query(2*t+1,m+1,r,val) : left;
            }

            int query(int val){
                // first leaf T where nodes[T] >= val
                return query(1,1,n,val);
            }
        };

    public:
        int numOfUnplacedFruits(vector<int>& fruits, vector<int>& baskets) {
            return numOfUnplacedFruitsSegtree(fruits, baskets);
        }

        int numOfUnplacedFruitsSegtree(vector<int>& fruits, vector<int>& baskets) {
            int n = fruits.size();
            segtree tree(baskets);

            int remain = n;
            for (auto& fruit : fruits){
                // find first basket T where baskets[T] >= fruit
                int id = tree.query(fruit);
                if (id < 0) continue;
                tree.update(id, 0);
                --remain;
            }
            return remain;
        }

        int numOfUnplacedFruitsTrivialMap(vector<int>& fruits, vector<int>& baskets) {
            // wrong algorithm
            int n = fruits.size();
            map<int, queue<int>> capa2id;
            for (int i = 0; i < n; ++i){
                capa2id[baskets[i]].push(i);
            }

            int remain = n;
            for (auto& fruit : fruits){
                // find first basket T where baskets[T] >= fruit
                auto it = capa2id.lower_bound(fruit);
                if (it == capa2id.end()) continue;
                int basket = baskets[it->second.front()];
                it->second.pop();
                if (it->second.empty()) capa2id.erase(it);
                --remain;
                cout << "fruit: " << fruit << ", basket: " << basket << ", remain: " << remain << endl;
            }
            return remain;
        }
};

 auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    vector<int> fruits = {4,2,5}, baskets = {3,5,4};
    fruits = {3,6,1}, baskets = {6,4,7};
    Solution s;
    cout << s.numOfUnplacedFruits(fruits, baskets) << endl;
    return 0;
}

