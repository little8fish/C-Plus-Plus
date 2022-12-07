/**
 * @file
 * @brief A generic [binary search tree](https://en.wikipedia.org/wiki/Binary_search_tree) implementation.
 * Here you can find more information about the algorithm: [Scaler - Binary Search tree](https://www.scaler.com/topics/data-structures/binary-search-tree/).
 * @see binary_search_tree.cpp
 */

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

/**
 * @brief The Binary Search Tree class.
 *
 * @tparam T The type of the binary search tree key.
 */
template <class T>
class binary_search_tree {
 private:
    /**
    * @brief A struct to represent a node in the Binary Search Tree.
    */
    struct bst_node {
        T value; /**< The value/key of the node. */
        std::unique_ptr<bst_node> left; /**< Pointer to left subtree. */
        std::unique_ptr<bst_node> right; /**< Pointer to right subtree. */

        /**
        * Constructor for bst_node, used to simplify node construction and
        * smart pointer construction.
        * @param _value The value of the constructed node.
        */
        explicit bst_node(T _value) {
            value = _value;
            left = nullptr;
            right = nullptr;
        }
    };

    std::unique_ptr<bst_node> root_; /**< Pointer to the root of the BST. */
    std::size_t size_ = 0; /**< Number of elements/nodes in the BST. */

    /**
     * @brief Recursive function to find the maximum value in the BST.
     *
     * @param node The node to search from.
     * @param ret_value Variable to hold the maximum value.
     * @return true If the maximum value was successfully found.
     * @return false Otherwise.
     */
    // 因为节点是unique_ptr，所以传指针的引用
    bool find_max(std::unique_ptr<bst_node>& node, T& ret_value) {
        if (!node) {
            return false;
        } else if (!node->right) { // 如果右为空，返回当前节点值
            ret_value = node->value; // 其实ret_value只是用来存值的
            return true;
        }
        return find_max(node->right, ret_value); // 递归
    }

    /**
     * @brief Recursive function to find the minimum value in the BST.
     *
     * @param node The node to search from.
     * @param ret_value Variable to hold the minimum value.
     * @return true If the minimum value was successfully found.
     * @return false Otherwise.
     */
    bool find_min(std::unique_ptr<bst_node>& node, T& ret_value) {
        if (!node) {
            return false;
        } else if (!node->left) {
            ret_value = node->value;
            return true;
        }

        return find_min(node->left, ret_value);
    }

    /**
     * @brief Recursive function to insert a value into the BST.
     *
     * @param node The node to search from.
     * @param new_value The value to insert.
     * @return true If the insert operation was successful.
     * @return false Otherwise.
     */
    bool insert(std::unique_ptr<bst_node>& node, T new_value) {
        if (root_ == node && !root_) { // 如果根节点为空
            root_ = std::unique_ptr<bst_node>(new bst_node(new_value));
            return true;
        }

        if (new_value < node->value) {
            if (!node->left) { // 如果左子树为空
                node->left = std::unique_ptr<bst_node>(new bst_node(new_value));
                return true;
            } else { // 递归的过程
                return insert(node->left, new_value);
            }
        } else if (new_value > node->value) {
            if (!node->right) {
                node->right =
                    std::unique_ptr<bst_node>(new bst_node(new_value));
                return true;
            } else {
                return insert(node->right, new_value);
            }
        } else { // 如果节点值相同就return false
            return false;
        }
    }

    /**
     * @brief Recursive function to remove a value from the BST.
     *
     * @param parent The parent node of node.
     * @param node The node to search from.
     * @param rm_value The value to remove.
     * @return true If the removal operation was successful.
     * @return false Otherwise.
     */
    bool remove(std::unique_ptr<bst_node>& parent,
                std::unique_ptr<bst_node>& node, T rm_value) {
        if (!node) {
            return false;
        }

        if (node->value == rm_value) { // 查找到值
            if (node->left && node->right) {
                T successor_node_value{}; // 这里是大括号初始化
                find_max(node->left, successor_node_value); // 找到左子树最大值
                remove(root_, root_, successor_node_value); // 移除左子树最大值的这个节点
                node->value = successor_node_value; // 把 = rm_value节点的值设为successor_node_value
                return true;
            } else if (node->left || node->right) { // 如果只有一个子节点
                std::unique_ptr<bst_node>& non_null = // 这里也是对指针的引用
                    (node->left ? node->left : node->right);

                if (node == root_) {
                    root_ = std::move(non_null); // std::move()将左值转换成右值,等价于root_(non_null.release())
                } else if (rm_value < parent->value) {
                    parent->left = std::move(non_null); // std::move是常见的转移unique_ptr控制权的方法，因为unique_ptr不支持 operator =
                } else {
                    parent->right = std::move(non_null);
                }

                return true;
            } else { // 当前节点为子节点
                if (node == root_) {
                    root_.reset(nullptr); // 其实root_.reset()会自动把root_设为nullptr
                } else if (rm_value < parent->value) {
                    parent->left.reset(nullptr); // 释放左节点
                } else {
                    parent->right.reset(nullptr);
                }
                return true;
            }
        } else if (rm_value < node->value) {
            return remove(node, node->left, rm_value); // 递归删除
        } else {
            return remove(node, node->right, rm_value);
        }
    }

    /**
     * @brief Recursive function to check if a value is in the BST.
     *
     * @param node The node to search from.
     * @param value The value to find.
     * @return true If the value was found in the BST.
     * @return false Otherwise.
     */
    // 查找值是否在树里
    bool contains(std::unique_ptr<bst_node>& node, T value) {
        if (!node) {
            return false;
        }

        if (value < node->value) {
            return contains(node->left, value);
        } else if (value > node->value) {
            return contains(node->right, value);
        } else {
            return true;
        }
    }

    /**
     * @brief Recursive function to traverse the tree in in-order order.
     *
     * @param callback Function that is called when a value needs to processed.
     * @param node The node to traverse from.
     */
    void traverse_inorder(std::function<void(T)> callback,
                          std::unique_ptr<bst_node>& node) {
        if (!node) {
            return;
        }

        traverse_inorder(callback, node->left);
        callback(node->value);
        traverse_inorder(callback, node->right);
    }

    /**
     * @brief Recursive function to traverse the tree in pre-order order.
     *
     * @param callback Function that is called when a value needs to processed.
     * @param node The node to traverse from.
     */
    void traverse_preorder(std::function<void(T)> callback,
                           std::unique_ptr<bst_node>& node) {
        if (!node) {
            return;
        }

        callback(node->value);
        traverse_preorder(callback, node->left);
        traverse_preorder(callback, node->right);
    }

    /**
     * @brief Recursive function to traverse the tree in post-order order.
     *
     * @param callback Function that is called when a value needs to processed.
     * @param node The node to traverse from.
     */
    void traverse_postorder(std::function<void(T)> callback,
                            std::unique_ptr<bst_node>& node) {
        if (!node) {
            return;
        }

        traverse_postorder(callback, node->left);
        traverse_postorder(callback, node->right);
        callback(node->value);
    }

 public:
    /**
     * @brief Construct a new Binary Search Tree object.
     *
     */
    // 构造函数
    binary_search_tree() {
        root_ = nullptr;
        size_ = 0;
    }

    /**
     * @brief Insert a new value into the BST.
     *
     * @param new_value The value to insert into the BST.
     * @return true If the insertion was successful.
     * @return false Otherwise.
     */
    // public借口
    bool insert(T new_value) {
        bool result = insert(root_, new_value);
        if (result) {
            size_++;
        }
        return result;
    }

    /**
     * @brief Remove a specified value from the BST.
     *
     * @param rm_value The value to remove.
     * @return true If the removal was successful.
     * @return false Otherwise.
     */
    // public接口
    bool remove(T rm_value) {
        bool result = remove(root_, root_, rm_value);
        if (result) {
            size_--;
        }
        return result;
    }

    /**
     * @brief Check if a value is in the BST.
     *
     * @param value The value to find.
     * @return true If value is in the BST.
     * @return false Otherwise.
     */
    bool contains(T value) { return contains(root_, value); }

    /**
     * @brief Find the smallest value in the BST.
     *
     * @param ret_value Variable to hold the minimum value.
     * @return true If minimum value was successfully found.
     * @return false Otherwise.
     */
    bool find_min(T& ret_value) { return find_min(root_, ret_value); }

    /**
     * @brief Find the largest value in the BST.
     *
     * @param ret_value Variable to hold the maximum value.
     * @return true If maximum value was successfully found.
     * @return false Otherwise.
     */
    bool find_max(T& ret_value) { return find_max(root_, ret_value); }

    /**
     * @brief Get the number of values in the BST.
     *
     * @return std::size_t Number of values in the BST.
     */
    std::size_t size() { return size_; }

    /**
     * @brief Get all values of the BST in in-order order.
     *
     * @return std::vector<T> List of values, sorted in in-order order.
     */
    std::vector<T> get_elements_inorder() {
        std::vector<T> result;
        traverse_inorder([&](T node_value) { result.push_back(node_value); }, // 需要用引用捕获，因为unique_ptr指针
                         root_);
        return result; // 返回装有value的vector容器
    }

    /**
     * @brief Get all values of the BST in pre-order order.
     *
     * @return std::vector<T> List of values, sorted in pre-order order.
     */
    std::vector<T> get_elements_preorder() {
        std::vector<T> result;
        traverse_preorder([&](T node_value) { result.push_back(node_value); },
                          root_);
        return result;
    }

    /**
     * @brief Get all values of the BST in post-order order.
     *
     * @return std::vector<T> List of values, sorted in post-order order.
     */
    std::vector<T> get_elements_postorder() {
        std::vector<T> result;
        traverse_postorder([&](T node_value) { result.push_back(node_value); },
                           root_);
        return result;
    }
};

/**
 * @brief Function for testing insert().
 * 
 * @returns `void`
 */
static void test_insert() {
    std::cout << "Testing BST insert...";

    binary_search_tree<int> tree;
    bool res = tree.insert(5);
    int min = -1, max = -1;
    assert(res);
    assert(tree.find_max(max));
    assert(tree.find_min(min));
    assert(max == 5);
    assert(min == 5);
    assert(tree.size() == 1);

    tree.insert(4);
    tree.insert(3);
    tree.insert(6);
    assert(tree.find_max(max));
    assert(tree.find_min(min));
    assert(max == 6);
    assert(min == 3);
    assert(tree.size() == 4);

    bool fail_res = tree.insert(4);
    assert(!fail_res);
    assert(tree.size() == 4);

    std::cout << "ok" << std::endl;
}

/**
 * @brief Function for testing remove().
 * 
 * @returns `void`
 */
static void test_remove() {
    std::cout << "Testing BST remove...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    bool res = tree.remove(5);
    int min = -1, max = -1;
    assert(res);
    assert(tree.find_max(max));
    assert(tree.find_min(min));
    assert(max == 6);
    assert(min == 3);
    assert(tree.size() == 3);
    assert(tree.contains(5) == false);

    tree.remove(4);
    tree.remove(3);
    tree.remove(6);
    assert(tree.size() == 0);
    assert(tree.contains(6) == false);

    bool fail_res = tree.remove(5);
    assert(!fail_res);
    assert(tree.size() == 0);

    std::cout << "ok" << std::endl;
}

/**
 * @brief Function for testing contains().
 * 
 * @returns `void`
 */
static void test_contains() {
    std::cout << "Testing BST contains...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    assert(tree.contains(5));
    assert(tree.contains(4));
    assert(tree.contains(3));
    assert(tree.contains(6));
    assert(!tree.contains(999));

    std::cout << "ok" << std::endl;
}

/**
 * @brief Function for testing find_min().
 * 
 * @returns `void`
 */
static void test_find_min() {
    std::cout << "Testing BST find_min...";

    int min = 0;
    binary_search_tree<int> tree;
    assert(!tree.find_min(min));

    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    assert(tree.find_min(min));
    assert(min == 3);

    std::cout << "ok" << std::endl;
}

/**
 * @brief Function for testing find_max().
 * 
 * @returns `void`
 */
static void test_find_max() {
    std::cout << "Testing BST find_max...";

    int max = 0;
    binary_search_tree<int> tree;
    assert(!tree.find_max(max));

    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    assert(tree.find_max(max));
    assert(max == 6);

    std::cout << "ok" << std::endl;
}

/**
 * @brief Function for testing get_elements_inorder().
 * 
 * @returns `void`
 */
static void test_get_elements_inorder() {
    std::cout << "Testing BST get_elements_inorder...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    std::vector<int> expected = {3, 4, 5, 6};
    std::vector<int> actual = tree.get_elements_inorder();
    assert(actual == expected);

    std::cout << "ok" << std::endl;
}

/**
 * @brief Function for testing get_elements_preorder().
 * 
 * @returns `void`
 */
static void test_get_elements_preorder() {
    std::cout << "Testing BST get_elements_preorder...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    std::vector<int> expected = {5, 4, 3, 6};
    std::vector<int> actual = tree.get_elements_preorder();
    assert(actual == expected);

    std::cout << "ok" << std::endl;
}

/**
 * @brief Function for testing get_elements_postorder().
 * 
 * @returns `void`
 */
static void test_get_elements_postorder() {
    std::cout << "Testing BST get_elements_postorder...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    std::vector<int> expected = {3, 4, 6, 5};
    std::vector<int> actual = tree.get_elements_postorder();
    assert(actual == expected);

    std::cout << "ok" << std::endl;
}

int main() {
    test_insert();
    test_remove();
    test_contains();
    test_find_max();
    test_find_min();
    test_get_elements_inorder();
    test_get_elements_preorder();
    test_get_elements_postorder();
}
