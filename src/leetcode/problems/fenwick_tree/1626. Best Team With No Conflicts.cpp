#include <vector>
#include <iostream>
#include <set>

using namespace std;

class Solution {
    private:
        using Score = int;
        using Index = int;

        struct player {
            int score;
            int age;
        };

        struct fenwicktree {
            int n;
            vector<int> bits;

            static inline constexpr int kNil = INT_MIN;

            int lsb(int x){
                return x & (-x);
            }

            fenwicktree(int n_) : n(n_), bits(n+1, kNil){}

            int query(int idx){
                if (idx < 0) return kNil;
                ++idx;
                int res = 0;
                while (idx > 0){
                    res = max(res, bits[idx]);
                    idx -= lsb(idx);
                }
                return res;
            }

            void update(int idx, int val){
                ++idx;
                while (idx <= n){
                    bits[idx] = max(bits[idx], val);
                    idx += lsb(idx);
                }
            }
        };

        unordered_map<Score, Index> compress(vector<int>& scores){
            sort(scores.begin(), scores.end());
            scores.erase(unique(scores.begin(), scores.end()), scores.end());
            unordered_map<Score, Index> compression;
            for (int i = 0; i < (int)scores.size(); ++i){
                compression[scores[i]] = i;
            }
            return compression;
        }

    public:
        int bestTeamScore(vector<int>& scores, vector<int>& ages) {
            return bestTeamScoreCoordinateCompression(scores, ages);
        }
        int bestTeamScoreNoCoordinateCompression(vector<int>& scores, vector<int>& ages) {
            int n = scores.size();
            vector<player> players(n);
            for (int i = 0; i < n; ++i){
                players[i] = {scores[i], ages[i]};
            }
            sort(players.begin(), players.end(), [](const player& lhs, const player& rhs){
                if (lhs.age < rhs.age) return true;
                return (lhs.age == rhs.age && lhs.score < rhs.score);
            });

            unordered_map<Score, Index> compression = compress(scores);
            fenwicktree tree(compression.size());

            int team_score = 0;
            for (int i = 0; i < n; ++i){
                int score       = players[i].score;
                int idx         = compression[score];
                int prev_score  = tree.query(idx);
                int curr_score  = prev_score + score;
                tree.update(idx, curr_score);
                team_score = max(team_score, curr_score);
            }

            return team_score;

        }
        int bestTeamScoreCoordinateCompression(vector<int>& scores, vector<int>& ages) {
            int n = scores.size();
            vector<player> players(n);
            for (int i = 0; i < n; ++i){
                players[i] = {scores[i], ages[i]};
            }
            sort(players.begin(), players.end(), [](const player& lhs, const player& rhs){
                if (lhs.age < rhs.age) return true;
                return (lhs.age == rhs.age && lhs.score < rhs.score);
            });

            fenwicktree tree(1e6);
            int team_score=0;
            // vector<int> dp(n, 0); // dp[i] = max score of a team with no conflict ends with player[i]
            for (int i = 0; i < n; ++i){
                int score = players[i].score;
                int q = tree.query(score);
                int curr_team_score = q + score;
                tree.update(score, curr_team_score);                
                team_score = max(team_score, curr_team_score);
            }
            return team_score;
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
    vector<int> scores = {1,3,5,10,15};
    vector<int> ages = {1,2,3,4,5};
    scores = {1,2,3,5};
    ages = {8,9,10,1};
    cout << sol.bestTeamScore(scores, ages) << endl; // 34
    return 0;
}