#include <iostream>
#include <vector>
#include <map>

using namespace std;

class LFUCache {
    private:
        struct node {
            int key{-1};
            int value{-1};
            int freq{0};
            node* prev{nullptr};
            node* next{nullptr};
        };

        struct bucket {
            node* head{nullptr};
            node* tail{nullptr};
            int element;

            bucket(){
                head = new node();
                tail = new node();
                head->next = tail;
                tail->prev = head;
                element = 0;
            }

            ~bucket(){
                delete head;
                delete tail;
            }

            int evict(){
                return remove_node(head->next);
            }

            int remove_node(node* curr){
                node* prev = curr->prev;
                node* next = curr->next;
                prev->next = next;
                next->prev = prev;
                --element;
                return curr->key;
            }

            int insert_node(node* curr){
                node* prev = tail->prev;
                prev->next = curr;
                curr->prev = prev;
                curr->next = tail;
                tail->prev = curr;
                ++element;
                return curr->key;
            }

            int size(){
                return element;
            }

            bool empty(){
                return element == 0;
            }
        };

        using Freq = int;
        unordered_map<Freq, bucket> buckets_;
        unordered_map<int, node*> key2node_;

        int element_;
        int capacity_;
        int min_freq_;

    private:
        void update_node(node* curr){
            int curr_freq = curr->freq;
            int next_freq = curr->freq + 1;
            bucket& curr_bucket = buckets_[curr_freq];
            bucket& next_bucket = buckets_[next_freq];

            curr_bucket.remove_node(curr);
            if (curr_bucket.empty()){
                min_freq_ = (min_freq_ == curr_freq) ? next_freq : min_freq_;
                buckets_.erase(curr_freq);
            }
            next_bucket.insert_node(curr);
            curr->freq = next_freq;
        }

        void evict_node(){
            bucket& b = buckets_[min_freq_];
            int key = b.evict();
            key2node_.erase(key);
            if (b.empty()) buckets_.erase(min_freq_);
        }

    public:
        LFUCache(int capacity) {
            capacity_ = capacity;
            element_ = 0;
            min_freq_ = 0;
        }
        
        int get(int key) {
            if (!key2node_.count(key)) return -1;
            node* curr = key2node_[key];
            update_node(curr);
            return curr->value;
        }
        
        void put(int key, int value) {
            if (key2node_.count(key)){
                node* curr = key2node_[key];
                update_node(curr);
                curr->value = value;
                return;
            }
            ++element_;
            int freq = 1;
            node* curr = new node({key, value, freq});
            key2node_[key] = curr;
            if (element_ > capacity_) evict_node();
            min_freq_ = freq;
            buckets_[freq].insert_node(curr);
        }
};

/**
 * Your LFUCache object will be instantiated and called as such:
 * LFUCache* obj = new LFUCache(capacity);
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
    LFUCache* obj = new LFUCache(2);
    // cout << "put(1, 1)" << endl; obj->put(1, 1);
    // cout << "put(2, 2)" << endl; obj->put(2, 2);
    // cout << "get(1): \t" << obj->get(1) << endl; // returns 1
    // cout << "put(3, 3)" << endl; obj->put(3, 3);
    // cout << "get(2): \t" << obj->get(2) << endl; // returns -1 (not found)
    // cout << "get(3): \t" << obj->get(3) << endl; // returns 3
    // cout << "put(4, 4)" << endl; obj->put(4, 4);
    // cout << "get(1): \t" << obj->get(1) << endl; // returns -1 (not found)
    // cout << "get(3): \t" << obj->get(3) << endl; // returns 3
    // cout << "get(4): \t" << obj->get(4) << endl; // returns 4

    cout << "put(3, 1)" << endl; obj->put(3, 1);
    cout << "put(2, 1)" << endl; obj->put(2, 1);
    cout << "put(2, 2)" << endl; obj->put(2, 2);
    cout << "put(4, 4)" << endl; obj->put(4, 4);
    cout << "get(2): \t" << obj->get(2) << endl; // returns 2
    return 0;
} 
