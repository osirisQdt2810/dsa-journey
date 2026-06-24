#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        using ll = long long;
        static inline constexpr int MOD = 1e9 + 7;

        static inline void accum(ll& a, ll b){    // a = a + b
            long long c = a + b;
            if (c >= MOD) c -= MOD;
            a = c;
        }

        static inline void accum(int& a, int b){    // a = a + b
            ll c = a + b;
            if (c >= MOD) c -= MOD;
            a = c;
        }

        static inline int sub(int a, int b){
            int c = a - b;
            if (c < 0) c += MOD;
            return c;
        }

        static inline int add(int a, int b){
            ll c = a + b;
            if (c >= MOD) c -= MOD;
            return c;
        }

        static inline int mul(int a, int b){
            ll c = 1LL * a * b;
            return c % MOD;
        }

        using Type = int;

        struct vec {
            int n;  // (n,1)
            vector<int> data;

            vec(int n_, int val=0) : n(n_), data(n_, val){}

            int operator+(const vec& other) const {
                return add(sum(), other.sum());
            }

            int sum() const {
                int s = 0;
                for (int i = 0; i < n; ++i){
                    accum(s, data[i]);
                }
                return s;
            }
        };

        struct matrix {
            int m, n;
            vector<vector<int>> data;
            matrix(int m_, int n_, int val=0) : m(m_), n(n_), data(m_, vector<int>(n_, val)){}

            matrix operator*(const matrix& other){
                int p = other.n;
                matrix Cmat(m, p, 0);
                // (m,n) * (n,p) = (m,n)
                auto& A = data;
                auto& B = other.data;
                auto& C = Cmat.data;
                for (int i = 0; i < m; ++i){
                    for (int k = 0; k < n; ++k){
                        for (int j = 0; j < p; ++j){
                            accum(C[i][j], mul(A[i][k], B[k][j]));
                        }
                    }
                }
                return Cmat;
            }

            matrix identity(){
                matrix I(m, n, 0);
                for (int i = 0; i < m; ++i){
                    I.data[i][i] = 1;
                }
                return I;
            }

            matrix pow(int n){
                if (n == 0) return identity();
                if (n == 1) return *this;
                auto mm = pow(n/2);
                auto mm_square = mm * mm;
                return (n%2) ? mm_square * (*this) : mm_square;
            }

            vec operator*(const vec& v){
                vec bvec(m, 0);
                auto& A = data;     // (m, n)
                auto& x = v.data;   // (n, 1)
                auto& b = bvec.data;   // (n, 1)
                for (int i = 0; i < m; ++i){
                    for (int k = 0; k < n; ++k){
                        accum(b[i], mul(A[i][k], x[k]));
                    }
                }
                return bvec;
            }
        };

        struct fenwicktree {
            int n;
            vector<Type> bits;   // bits[i]: lưu trữ thông tin từ (i-k, i], trong đó k=2^(least significant bit) của i
                                // vd: bits[1]: [1]         (i=1=1 => k=1)
                                // vd: bits[2]: [1,2]       (i=2=10 => k=2)
                                // vd: bits[3]: [3]         (i=3=11 => k=1)
                                // vd: bits[4]: [1,2,3,4]   (i=4=100 => k=4)
                                // vd: bits[5]: [5,5]       (i=5=101 => k=1)
                                // vd: bits[6]: [5,6]       (i=6=110 => k=2)
                                // vd: bits[7]: [7]         (i=7=111 => k=1)
                                // vd: bits[8]: [1,2...8]   (i=8=1000 => k=8)
                                // T = i & (-i) = i - k
                                // sum[i] = bit[i] + bit[i&(-i)] + ...
            fenwicktree(int n_) : n(n_), bits(n+1, 0){}

            void build(Type val){    // update all a[i]=1
                for (int i = 1; i <= n; ++i){
                    accum(bits[i], val);
                    int p = i + (i & (-i));
                    if (p <= n) accum(bits[p], bits[i]);
                }
            }

            void update(int i, Type val){ // update a[v] += val;
                while (i <= n){
                    accum(bits[i], val);
                    i += i & (-i);
                }
            }

            Type query(int l){   // query a[1...l]
                Type res = 0;
                while (l){
                    accum(res, bits[l]);
                    l -= l & (-l);
                }
                return res;
            }

            Type query(int l, int r){    // query a[l...r]
                Type val = query(r) - query(l-1);
                return (val < 0) ? (val + MOD) : val;
            }

        };

    public:
        int zigZagArraysNaive(int n, int l, int r) { // TLE: O(n.v^2)
            const int MAX_N = 200;
            const int DIRECTIONS = 2;
            const int MAX_VAL = 200;
            int dp[MAX_N][DIRECTIONS][MAX_VAL]   ;   // dp[n][directions][T]: number of zigzag arrays length n, where x[n] = T and have directions up or down
            memset(dp, 0, sizeof(dp));

            for (int d = 0; d < DIRECTIONS; ++d){
                for (int v = l; v <= r; ++v){
                    dp[1][d][v] = 1;
                }
            }
            // O(n*v^2) where v is the max value
            // l = 0;
            // r = r-l;
            for (int i = 2; i <= n; ++i){
                for (int d = 0; d < DIRECTIONS; ++d){
                    for (int v = l; v <= r; ++v){
                        // dp[i][d][v], x[i] = v
                        int tl, tr;
                        if (d == 0){    // up: x[i-1] < v => dp[i][d][v] = sum(dp[i-1][1-d][t]), where l <= t < v
                            tl = l;
                            tr = v-1;
                        }
                        else {  // down: x[i-1] > v => dp[i][d][v] = sum(dp[i-1][1-d][t]), where v < t <= r
                            tl = v+1;
                            tr = r;
                        }
                        for (int t = tl; t <= tr; ++t){
                            accum(dp[i][d][v], dp[i-1][1-d][t]);
                        }
                    }
                }
            }

            int res = 0;
            for (int d = 0; d < DIRECTIONS; ++d){
                for (int v = l; v <= r; ++v){
                    accum(res, dp[n][d][v]);
                }
            }
            return res;
        }

        int zigZagArrays(int n, int l, int r) {
            // define:
            //      - dp[i][0][v], 1 <= v <= T, down
            //      - dp[i][1][v], 1 <= v <= T, up
            // we have:
            //      - dp[i][0][v] = sigma(dp[i-1][1][t]), where v+1<=t<=T
            //      - dp[i][1][v] = sigma(dp[i-1][0][t]), where 1<=t<=v-1
            // represent as matrix:
            //      - call Ac = dp[i][0], Bc = dp[i][1], Ap = dp[i-1][0], Bp = dp[i-1][1]
            //      - A, B shape: (T, 1)
            //      - then:
            //          - Ac = X * Bp
            //          - Bc = Y * Ap
            //      - where:
            //          - X = [0 1 1 ... 1] -> (T, T)
            //                [0 0 1 ... 1]
            //                .............
            //                [0 0 ..... 0]
            //          - Y = [0 0 0 ... 0] -> (T, T)
            //                [1 0 0 ... 0]
            //                .............
            //                [1 1 ..... 0]
            //      - then:
            //          - An = (X * Bn-1) = (X*Y) * An-2 = Z * An-2 = Z^k * Am (k=(n-1)/2, m=(2-n%2))
            //          - Bn = (Y * An-1) = (Y*X) * Bn-2 = T * Bn-2 = T^k * Bm (k=(n-1)/2, m=(2-n%2))
            //          - A1 = B1 = vector of 1
            //          - A2 = X*B1, B2 = Y*A1
            //      - e.g.:
            //          - A1 = B1 = I
            //          - A2 = Z^0 * A0 = X, B2 = Z^0 * B0 = Y
            //          - A3 = Z, B3 = T
            //          - A4 = Z * X, B4 = T * Y
            r = r - l + 1;
            l = 0;
            matrix X(r, r), Y(r, r);
            // build X, Y
            for (int i = 0; i < r; ++i){
                for (int j = i + 1; j < r; ++j){
                    X.data[i][j] = 1;
                }
                for (int j = 0; j < i; ++j){
                    Y.data[i][j] = 1;
                }
            }

            int k = (n-1)/2, m = (2-n%2);
            matrix Z = X * Y;
            matrix T = Y * X;

            vec A1(r, 1);
            vec B1(r, 1);
            vec A2 = X*B1;
            vec B2 = Y*A1;
            vec Am = (m == 1) ? A1 : A2;
            vec Bm = (m == 1) ? B1 : B2;
            vec An = Z.pow(k) * Am;
            vec Bn = T.pow(k) * Bm;
            return An + Bn;
        }

        int zigZagArraysPrefixsum(int n, int l, int r) { // TLE: O(n.v), using prefix sum and
            const int DIRECTIONS = 2;
            const int MAX_VAL = 200;
            auto build_array = [&](){
                return vector<vector<int>>(DIRECTIONS, vector<int>(MAX_VAL, 0));
            };
            
            auto dp = build_array();
            auto prev = build_array();

            for (int d = 0; d < DIRECTIONS; ++d){
                for (int v = l; v <= r; ++v){
                    dp[d][v] = 1;
                    prev[d][v] = add(prev[d][v-1], dp[d][v]);
                }
            }

            for (int i = 2; i <= n; ++i){
                auto curr = build_array();
                for (int d = 0; d < DIRECTIONS; ++d){
                    for (int v = l; v <= r; ++v){
                        // dp[i][d][v], x[i] = v
                        int tl, tr;
                        if (d == 0){    // up: x[i-1] < v => dp[i][d][v] = sum(dp[i-1][1-d][t]), where l <= t < v
                            tl = l;
                            tr = v-1;
                        }
                        else {  // down: x[i-1] > v => dp[i][d][v] = sum(dp[i-1][1-d][t]), where v < t <= r
                            tl = v+1;
                            tr = r;
                        }
                        dp[d][v] = sub(prev[1-d][tr], prev[1-d][tl-1]);
                        curr[d][v] = add(curr[d][v-1], dp[d][v]);
                    }
                }
                prev = std::move(curr);
            }

            Type res = 0;
            for (int d = 0; d < DIRECTIONS; ++d){
                accum(res, sub(prev[d][r], prev[d][l-1]));
            }
            return res;
        }

        int zigZagArraysFenwick(int n, int l, int r) { // TLE: O(n.vlogv), using fenwicktree to query
            const int MAX_N = 10;
            const int DIRECTIONS = 2;
            const int MAX_VAL = 10;
            // int dp[MAX_N][DIRECTIONS][MAX_VAL]   ;   // dp[n][directions][T]: number of zigzag arrays length n, where x[n] = T and have directions up or down
            // memset(dp, 0, sizeof(dp));
            r = r - l + 1;
            l = 1;
            vector<vector<fenwicktree>> forests(n+1, vector<fenwicktree>(DIRECTIONS, r));
            for (int d = 0; d < DIRECTIONS; ++d){
                forests[1][d].build(1);
            }

            // for (int d = 0; d < DIRECTIONS; ++d){
            //     for (int v = l; v <= r; ++v){
            //         dp[1][d][v] = 1;
            //     }
            // }
            for (int i = 2; i <= n; ++i){
                for (int d = 0; d < DIRECTIONS; ++d){
                    for (int v = l; v <= r; ++v){
                        // dp[i][d][v], x[i] = v
                        int tl, tr;
                        if (d == 0){    // up: x[i-1] < v => dp[i][d][v] = sum(dp[i-1][1-d][t]), where l <= t < v
                            tl = l;
                            tr = v-1;
                        }
                        else {  // down: x[i-1] > v => dp[i][d][v] = sum(dp[i-1][1-d][t]), where v < t <= r
                            tl = v+1;
                            tr = r;
                        }
                        forests[i][d].update(v, forests[i-1][1-d].query(tl, tr));
                    }
                }
            }

            Type res = 0;
            for (int d = 0; d < DIRECTIONS; ++d){
                accum(res, forests[n][d].query(l, r));
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
    int n = 8, l = 4, r = 33;
    cout << sol.zigZagArraysNaive(n,l,r) << endl;
    cout << sol.zigZagArraysFenwick(n,l,r) << endl;
    cout << sol.zigZagArraysPrefixsum(n,l,r) << endl;
    cout << sol.zigZagArrays(n,l,r) << endl;
    return 0;
}