#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    int pairSum(ListNode* head) {
        // O(1) space
        int n = 0; ListNode* tmp = head;
        while (tmp){ ++n; tmp = tmp->next; }

        //
        if (n==2) return head->val + head->next->val;
        ListNode* prev = head;
        ListNode* curr = head->next;
        ListNode* next;
        n = n/2-1;
        while (true){
            next = curr->next;
            curr->next = prev;
            if (--n <= 0) break;
            prev = curr;
            curr = next;
        }
        head->next = nullptr;
        ListNode* curr_twin = next;

        int ans = 0;
        while (curr_twin != nullptr){
            ans = max(ans, curr->val + curr_twin->val);
            curr = curr->next;
            curr_twin = curr_twin->next;
        }
        return ans;

    }

    int pairSumSpaceBigON(ListNode* head) {
        // O(n) space
        vector<int> nodes;
        while (head){
            nodes.push_back(head->val);
            head = head->next;
        }
        int l = 0, r = nodes.size() - 1, ans = 0;
        while (l < r){
            ans = max(ans, nodes[l++] + nodes[r--]);
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
    Solution sol;
    return 0;
}