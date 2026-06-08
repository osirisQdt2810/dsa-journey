#include <iostream>
#include <vector>
#include <map>

using namespace std;

using Value_t = int;
using ll = long long;

class Fancy {
    private:
        int n{0};
        vector<int> sequences;
        static inline constexpr int MAX_N = 1e5 + 5;
        static inline constexpr int MOD = 1e9 + 7;

        static inline int add_mod(int a, int b){
            return (int)((ll(a) + b) % MOD);
        }

        static inline int mul_mod(int a, int b){
            return (int)((ll(a) * b) % MOD);
        }

        struct segtree {
            struct node {
                Value_t it{0};

                node operator+(const node& other){
                    return {add_mod(it, other.it)};
                }

                node operator+(Value_t o){
                    return {add_mod(it, o)};
                }

                node operator*(Value_t o){
                    return {mul_mod(it, o)};
                }
            };

            struct lazy{
                Value_t mul{1};
                Value_t add{0};

                lazy operator+(Value_t o){
                    return {mul, add_mod(add, o)};
                }

                lazy operator*(Value_t o){
                    return {mul_mod(mul, o), mul_mod(add, o)};
                }

                lazy operator+(const lazy& other){
                    // (a.x+y)*z+t
                    return {
                        mul_mod(mul, other.mul),
                        add_mod(mul_mod(add, other.mul), other.add)
                    };
                }
            };

            int n;
            vector<node> nodes;
            vector<lazy> laziness;

            segtree(int n_): n(n_), nodes(4*n_+1), laziness(4*n_+1){}

            void push_down(int t, int l, int r){
                if (laziness[t].add == 0 && laziness[t].mul == 1) return;

                Value_t lazy_add = laziness[t].add;
                Value_t lazy_mul = laziness[t].mul;
                int m = (l+r)/2;

                // update children:
                nodes[2*t]      = nodes[2*t] * lazy_mul + mul_mod(lazy_add, m-l+1);
                laziness[2*t]   = laziness[2*t] + laziness[t];

                nodes[2*t+1]      = nodes[2*t+1] * lazy_mul + mul_mod(lazy_add, r-m);
                laziness[2*t+1]   = laziness[2*t+1] + laziness[t];
                laziness[t] = {1, 0};
            }

            void update(int t, int l, int r, int pos, int val){
                // case 1: [l...r] pos or pos [l...r]
                if (r < pos || pos < l) return;
                // case 2: [pos...l...r...pos]
                if (l == r){
                    nodes[t] = {val};
                    return;
                }
                // case 3: overlap
                push_down(t,l,r);
                int m = (l+r)/2;
                update(2*t,l,m,pos,val);
                update(2*t+1,m+1,r,pos,val);
                // not need to update nodes[t]?
                nodes[t] = nodes[2*t] + nodes[2*t+1];
            }

            Value_t query(int t, int l, int r, int X, int Y){
                // case 1: [l...r] [X...Y] or [X...Y] [l...r]
                if (r < X || Y < l) return 0;
                // case 2: [X...l...r...Y]
                if (X <= l && r <= Y) return nodes[t].it;
                // case 3: overlap
                push_down(t,l,r);
                int m = (l+r)/2;
                return query(2*t,l,m,X,Y) + query(2*t+1,m+1,r,X,Y);
            }

            void add(int t, int l, int r, int X, int Y, int inc){
                // case 1: [l...r] [X...Y] or [X...Y] [l...r]
                if (r < X || Y < l) return;
                // case 2: [X...l...r...Y]
                if (X <= l && r <= Y){
                    nodes[t] = nodes[t] + mul_mod(inc, r-l+1);
                    laziness[t] = laziness[t] + inc;
                    return;
                }
                // case 3: overlap
                push_down(t,l,r);
                int m = (l+r)/2;
                add(2*t,l,m,X,Y,inc);
                add(2*t+1,m+1,r,X,Y,inc);
                nodes[t] = nodes[2*t] + nodes[2*t+1];
            }

            void mul(int t, int l, int r, int X, int Y, int multiply){
                // case 1: [l...r] [X...Y] or [X...Y] [l...r]
                if (r < X || Y < l) return;
                // case 2: [X...l...r...Y]
                if (X <= l && r <= Y){
                    nodes[t] = nodes[t] * multiply;
                    laziness[t] = laziness[t] * multiply;
                    return;
                }
                // case 3: overlap
                push_down(t,l,r);
                int m = (l+r)/2;
                mul(2*t,l,m,X,Y,multiply);
                mul(2*t+1,m+1,r,X,Y,multiply);
                nodes[t] = nodes[2*t] + nodes[2*t+1];
            }

            void update(int pos, int val){
                update(1,1,n,pos,val);
            }

            int query(int l, int r){
                if (l > r || l > n) return -1;
                return query(1,1,n,l,r);
            }

            void add(int l, int r, int inc){
                add(1,1,n,l,r,inc);
            }

            void mul(int l, int r, int multiply){
                mul(1,1,n,l,r,multiply);
            }

        };

        segtree* tree;

    public:
        Fancy() {
            sequences.resize(MAX_N);
            tree = new segtree(MAX_N);
        }
        
        void append(int val) {
            ++n;
            tree->update(n, val);
        }
        
        void addAll(int inc) {
            tree->add(1, n, inc);
        }
        
        void multAll(int m) {
            tree->mul(1, n, m);
        }
        
        int getIndex(int idx) {
            if (idx + 1 > n) return -1;
            return tree->query(idx+1, idx+1);
        }
};

/**
 * Your Fancy object will be instantiated and called as such:
 * Fancy* obj = new Fancy();
 * obj->append(val);
 * obj->addAll(inc);
 * obj->multAll(m);
 * int param_4 = obj->getIndex(idx);
 */

 auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    // Fancy fancy = new Fancy();
    // fancy.append(2);   // fancy sequence: [2]
    // fancy.addAll(3);   // fancy sequence: [2+3] -> [5]
    // fancy.append(7);   // fancy sequence: [5, 7]
    // fancy.multAll(2);  // fancy sequence: [5*2, 7*2] -> [10, 14]
    // fancy.getIndex(0); // return 10
    // fancy.addAll(3);   // fancy sequence: [10+3, 14+3] -> [13, 17]
    // fancy.append(10);  // fancy sequence: [13, 17, 10]
    // fancy.multAll(2);  // fancy sequence: [13*2, 17*2, 10*2] -> [26, 34, 20]
    // fancy.getIndex(0); // return 26
    // fancy.getIndex(1); // return 34
    // fancy.getIndex(2); // return 20

    // Fancy* obj = new Fancy();
    // obj->append(2);
    // obj->addAll(3);
    // obj->append(7);
    // obj->multAll(2);
    // cout << obj->getIndex(0) << endl;
    // obj->addAll(3);
    // obj->append(10);
    // obj->multAll(2);
    // cout << obj->getIndex(0) << endl;
    // cout << obj->getIndex(1) << endl;
    // cout << obj->getIndex(2) << endl;
    
    // Fancy* obj = new Fancy();
    // obj->append(2);
    // obj->append(5);
    // obj->append(7);
    // obj->append(6);
    // obj->addAll(8);
    // obj->addAll(5);
    // cout << obj->getIndex(0) << endl;
    // obj->addAll(5);
    // cout << obj->getIndex(3) << endl;
    Fancy f;
    f.append(2);
    f.addAll(3);
    f.append(7);
    cout << f.getIndex(1);

    return 0;
}

