#include <vector>
#include <iostream>
#include <set>

using namespace std;

class Solution {
    private:
        struct fenwicktree {
            int n;
            vector<int> bits;

            int lsb(int i){
                return i & (-i);
            }

            void build(const vector<int>& arr){
                for (int i = 1; i <= n; ++i){
                    bits[i] += arr[i];
                    int parent = i + lsb(i);
                    if (parent <= n){
                        bits[parent] += bits[i];
                    }
                }
            }

            fenwicktree(const vector<int>& arr_, int n_) : n(n_), bits(n_+1, 0){
                build(arr_);
            }

            int query(int idx){
                int sum=0;
                while (idx){
                    sum += bits[idx];
                    idx -= lsb(idx);
                }
                return sum;
            }

            void inc(int pos, int delta){
                while (pos <= n){
                    bits[pos] += delta;
                    pos += lsb(pos);
                }
            }
        };

    public:
        vector<int> processQueries(vector<int>& queries, int m) {
            return processQueriesFenwickTree(queries, m);
        }

        vector<int> processQueriesFenwickTree(vector<int>& queries, int m) {
            int Q = queries.size();
            // giả sử ta store array sau Q queries như sau:
            //      ARR = q[Q] q[Q-1] ... q[2] q[1] 1 2 ... m
            // để tìm vị trí của 1 giá trị query T, ta tính số lượng các phần tử trước vị trí của T.
            // để ý: số lượng <=> tổng các giá trị 1
            // do đó, ta tạo ra 1 mảng X có cùng số lượng với ARR, trong đó X[i] = 1 thể hiện rằng tại X[i] đã có phần tử được reserve
            // ban đầu:
            //       X  = 0     0     ...   0   0  1 1 ... 1
            // như vậy, giả sử tại vòng lặp thứ i của queries, ta cần tìm giá trị query u
            // ta tìm vị trí của u trên ARR => có thể sử dụng 1 hash-table map đơn giản, giả sử vị trí là pos in [1, Q + m]
            // ta đơn giản tính số lượng các phần tử X[1...pos-1] là sẽ biết ngay vị trí của u trên mảng permutation hiện tại
            // sau đó, ta cập nhật X[q[i]] = 1
            vector<int> visited(Q + m + 1, 0);
            unordered_map<int, int> coordination;
            for (int i = 1; i <= m; ++i){
                visited[i+Q] = 1;
                coordination[i] = i+Q;
            }
            fenwicktree tree(visited, Q+m);
            for (int q = 0; q < Q; ++q){
                int u = queries[q];
                int target_pos = Q - q;
                // mapping from current query to index in [1, Q+m+1]
                int idx = coordination[u];
                queries[q] = tree.query(idx-1);

                // update pos after querying
                coordination[u] = target_pos;
                tree.inc(target_pos, 1);
                tree.inc(idx, -1);
            }
            return queries;
        }

        vector<int> processQueriesBruteForce(vector<int>& queries, int m) {
            int Q = queries.size();
            vector<int> nums(m);
            for (int i = 0; i < m; ++i){
                nums[i] = i+1;
            }
            vector<int> res(Q);
            for (int q = 0; q < Q; ++q){
                // nums: [a[1] a[2] ... a[m]]
                // find t where a[t] == queries[q]
                for (int i=0; i < m; ++i){
                    if (nums[i] == queries[q]){
                        res[q] = i;
                        break;
                    }
                }

                // nums: [a[1] a[2] ... a[t]   ... a[m]]
                // move: [a[t] a[1] ... a[t-1] ... a[m]]
                int t = res[q], tmp = nums[t];
                for (int i=t-1; i>=0; --i){
                    nums[i+1] = nums[i];
                }
                nums[0] = tmp;
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
    return 0;
}