#include <iostream>
#include <vector>
#include <map>

using namespace std;

class LRUCache {
    private:
        struct node {
            int key{-1};
            int data{-1};
            node* prev{nullptr};
            node* next{nullptr};
        };

        node* head_{nullptr};
        node* tail_{nullptr};

        int capacity_;
        int element_;
        unordered_map<int, node*> key2node_;

    private:
        void remove_link(node* curr_node){
            node* prev = curr_node->prev;
            node* next = curr_node->next;
            prev->next = next;
            next->prev = prev;
        }

        void update_tail(node* curr_node){
            node* prev = tail_->prev;
            prev->next = curr_node;
            curr_node->prev = prev;
            curr_node->next = tail_;
            tail_->prev = curr_node;
        }

        void update(node* curr_node){
            remove_link(curr_node);
            update_tail(curr_node);
        }

        void update_head(node* curr_node){
            head_->next = curr_node;
            curr_node->prev = head_;
            curr_node->next = tail_;
            tail_->prev = curr_node;
        }

        void evict(){
            int key = head_->next->key;
            remove_link(head_->next);
            key2node_.erase(key);
            --element_;
        }

    public:
        LRUCache(int capacity) : head_(new node()), tail_(new node()), capacity_(capacity), element_(0){
            head_->next = tail_;
            tail_->prev = head_;
        }
        
        int get(int key) {
            if (!key2node_.count(key)) return -1;
            node* curr = key2node_[key];
            update(curr);
            return curr->data;
        }
        
        void put(int key, int value) {
            if (key2node_.count(key)){
                node* curr = key2node_[key];
                curr->data = value;
                update(curr);
                return;
            }
            node* curr = new node({key, value});
            key2node_[key] = curr;
            ++element_;
            if (element_ == 1){
                update_head(curr);
                return;
            }
            if (element_ > capacity_) evict();
            // update tail
            update_tail(curr);
        }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */

 auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    LRUCache* obj = new LRUCache(2);
    cout << "put(1, 0)" << endl; obj->put(1, 0);
    cout << "put(2, 2)" << endl; obj->put(2, 2);
    cout << "get(1): \t" << obj->get(1) << endl; // returns 1
    cout << "put(3, 3)" << endl; obj->put(3, 3);
    cout << "get(2): \t" << obj->get(2) << endl; // returns -1 (not found)
    cout << "put(4, 4)" << endl; obj->put(4, 4);
    cout << "get(1): \t" << obj->get(1) << endl; // returns -1 (not found)
    cout << "get(3): \t" << obj->get(3) << endl; // returns -1 (not found)
    cout << "get(4): \t" << obj->get(4) << endl; // returns 4
    return 0;
} 
