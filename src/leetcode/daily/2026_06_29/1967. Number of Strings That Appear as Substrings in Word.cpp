#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        struct trie {
            struct node {
                int occurences{0};
                node* child[26];

                node(){ for (int i = 0; i < 26; ++i) child[i] = nullptr; }
            };

            node* root;

            trie(string& word) : root(new node()){ insert(word); }

            void insert(string& word){
                node* curr = root;
                for (auto w : word){
                    int c = (int)(w - 'a');
                    if (!curr->child[c]) curr->child[c] = new node();
                    curr = curr->child[c];
                    curr->occurences++;
                }
            }

            bool seek(string& word){
                node* curr = root;
                for (auto w : word){
                    int c = (int)(w - 'a');
                    if (!curr->child[c]) return false;
                    curr = curr->child[c];
                }
                return (curr->occurences);
            }

        };

    public:
        int numOfStrings(vector<string>& patterns, string word) {
            // trie tree(word);
            int res = 0;
            for (auto& pattern : patterns)
                res += word.contains(pattern);
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