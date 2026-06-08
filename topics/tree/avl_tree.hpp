#ifndef DSA_TREE_AVL_TREE_HPP
#define DSA_TREE_AVL_TREE_HPP

#include "tree/binary_search_tree.hpp"

#include <algorithm>
#include <vector>

namespace dsa::tree {

/**
 * @brief Self-balancing AVL tree storing unique keys.
 */
template <typename T>
class AvlTree : public BinarySearchTree<T> {
private:
    using Node = typename BinarySearchTree<T>::Node;

    /**
     * @brief Returns the height of `node`, treating null as height 0.
     */
    int height(Node* node) {
        return (node == nullptr) ? 0 : node->height;
    }

    /**
     * @brief Returns left subtree height minus right subtree height.
     */
    int balance(Node* node) {
        return (node == nullptr) ? 0 : height(node->left) - height(node->right);
    }

    /**
     * @brief rotate tree from left to right, which w
     *
     *           z                   y
     *          / \                 / \
     *         y   P    ------>    M   z
     *        / \                     / \
     *       M   N                   N   P
     */
    Node* rotate_right(Node* z) {
        Node* y = z->left;
        Node* n = y->right;

        y->right = z;
        z->left = n;
        z->height = 1 + std::max(height(z->left), height(z->right));
        y->height = 1 + std::max(height(y->left), height(y->right));
        return y;
    }

    /**
     * @brief rotate tree from left to right, which w
     *           z                   y
     *          / \                 / \
     *         M   y    ------>    z   P
     *            / \             / \
     *           N   P           M   N
     */
    Node* rotate_left(Node* z) {
        Node* y = z->right;
        Node* n = y->left;

        y->left = z;
        z->right = n;
        z->height = 1 + std::max(height(z->left), height(z->right));
        y->height = 1 + std::max(height(y->left), height(y->right));
        return y;
    }

protected:
    /**
     * @brief Inserts `key` and rebalances the affected subtree.
     */
    Node* insert(Node* parent, T key) override {
        if (parent == nullptr) return new Node(key);
        if (parent->value == key) return parent;

        if (parent->value < key) {
            parent->right = insert(parent->right, key);
        } else {
            parent->left = insert(parent->left, key);
        }

        // Balance the node* `parent`
        parent->height = 1 + std::max(height(parent->left), height(parent->right));
        int current_balance = balance(parent);

        /*
            There are two main cases that could happen to this node *parent.
             1. Case 1: Tree is unbalanced in left-side (balance > 1)
                        z --> parent
                       / \
                      y   P
                     / \
                    Y1 Y2
                In this first case, we have known that (by recursion) `y` is balanced, and `z` (parent) is unbalanced in left-side. So,
                the inserted node (with key valued `key`) caused the unbalanced may be in `Y1` or `Y2`, corresponding to two subcases
                1.1 and 1.2 in the following.

             2. Case 2: Tree is unbalanced in right-side (balance < -1)
                        z --> parent
                       / \
                      M   y
                         / \
                        Y1 Y2
                In this second case, we have known that (by recursion) `y` is balanced, and `z` (parent) is unbalanced in right-side. So,
                the inserted node (with key valued `key`) caused the unbalanced may be in `Y1` or `Y2`, corresponding to two subcases
                2.1 and 12.2 in the following.
        */

        /*
         * @brief case 1.1, the inserted node cause unbalancing for parent `z` places in `Y1` <=> Left Left Side case
                        z --> parent                                                y
                       / \                                                         / \
                      y   P                 Right Rotate on node z                x   z
                     / \                ------------------------------>          / \ / \
                    x  Y2                                                       M  N Y2 P
                   / \
                  M   N --> the inserted will be in (x, M or N)
         */
        if (current_balance > 1 && key < parent->left->value) {
            return rotate_right(parent);
        }

        /*
         * @brief case 1.2, the inserted node cause unbalancing for parent `z` places in `Y2` <=> Left Right Side case
                        z --> parent                                                z                                                    x
                       / \                                                         / \                                                  / \
                      y   P                    Left Rotate on node y              x   P              Right Rotate on node z            y   z
                     / \                  ------------------------------>        / \            ------------------------------>       / \ / \
                    Y1  x                                                       y   N                                                Y1 M N  P
                       / \                                                     / \
                      M   N --> the inserted will be in (x, M or N)           Y1  M
         */
        if (current_balance > 1 && key > parent->left->value) {
            parent->left = rotate_left(parent->left);
            return rotate_right(parent);
        }

        /*
         * @brief case 2.1, the inserted node cause unbalancing for parent `z` places in `Y2` <=> Right Right Side case
                        z --> parent                                                    y
                       / \                                                             / \
                      M   y                     Left Rotate on node z                 z   x
                         / \                ------------------------------>          / \ / \
                        Y1  x                                                       M Y1 N  P
                           / \
                          N   P --> the inserted will be in (x, N or P)
         */
        if (current_balance < -1 && key > parent->right->value) {
            return rotate_left(parent);
        }

        /*
         * @brief case 2.2, the inserted node cause unbalancing for parent `z` places in `Y1` <=> Right Left Side case
                        z --> parent                                                z                                                    x
                       / \                                                         / \                                                  / \
                      M   y                    Right Rotate on node y             M   x                 Left Rotate on node z          z   y
                         / \                  ------------------------------>        / \            ------------------------------>   / \ / \
                        x   Y2                                                      N   y                                            M  N P Y2
                       / \                                                             / \
                      N   P --> the inserted will be in (x, N or P)                   P  Y2
         */
        if (current_balance < -1 && key < parent->right->value) {
            parent->right = rotate_right(parent->right);
            return rotate_left(parent);
        }

        return parent;
    }

    /**
     * @brief Removes `key` and rebalances the affected subtree.
     */
    Node* remove(Node* parent, T key) override {
        if (parent == nullptr) return nullptr;
        if (parent->value < key) {
            parent->right = remove(parent->right, key);
        } else if (parent->value > key) {
            parent->left = remove(parent->left, key);
        } else {
            if (parent->left != nullptr && parent->right != nullptr) {
                Node* successor = this->minimum_node(parent->right);
                parent->value = successor->value;
                parent->right = remove(parent->right, successor->value);
            } else {
                Node* child = (parent->left != nullptr) ? parent->left : parent->right;
                delete parent;
                return child;
            }
        }

        parent->height = 1 + std::max(height(parent->left), height(parent->right));
        int current_balance = balance(parent);

        if (current_balance > 1 && balance(parent->left) >= 0) {
            return rotate_right(parent);
        }
        if (current_balance > 1 && balance(parent->left) < 0) {
            parent->left = rotate_left(parent->left);
            return rotate_right(parent);
        }
        if (current_balance < -1 && balance(parent->right) <= 0) {
            return rotate_left(parent);
        }
        if (current_balance < -1 && balance(parent->right) > 0) {
            parent->right = rotate_right(parent->right);
            return rotate_left(parent);
        }

        return parent;
    }

public:
    using BinarySearchTree<T>::insert;
    using BinarySearchTree<T>::remove;
    
    /**
     * @brief Creates an empty AVL tree.
     */
    AvlTree() = default;

    /**
     * @brief Creates an AVL tree by inserting all values in order.
     */
    explicit AvlTree(const std::vector<T>& values) : BinarySearchTree<T>(values) {}
};

}  // namespace dsa::tree

#endif  // DSA_TREE_AVL_TREE_HPP
