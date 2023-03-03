#ifndef TBAGO_SKIP_LIST_HPP_
#define TBAGO_SKIP_LIST_HPP_

#include <cstdlib>
#include <mutex>

template <typename K, typename V>
class SkipList;

// skiplist kv store node
template <typename K, typename V>
class Node {
public:
    explicit Node(K key, V value, int level) : key_(key), value_(value) {
        levels_ = new Node<K, V> *[level];
        std::memset(levels_, 0, sizeof(Node<K, V> *) * level);
    }
    ~Node() { delete[] levels_; }

public:
    K GetKey() const { return key_; }
    V GetValue() const { return value_; }
    void SetValue(V value) { value_ = value; }
    friend SkipList<K, V>;

private:
    K key_;
    V value_;
    Node<K, V> **levels_;  // store levels pointers
};                         // class Node

template <typename K, typename V>
class SkipList {
public:
    SkipList(int max_level);
    ~SkipList();
    SkipList(const SkipList&) = delete;
    SkipList & operator=(const SkipList &) = delete;
public:
    /**
     * insert kv to the skip list
     * return false when the key exists
     */
    bool InsertElement(const K key, const V value);
    bool SearchElement(const K key, V &value);
    void DeleteElement(const K key);
    int GetElementCount() const { return element_count_; }

private:
    Node<K, V> *CreateNode(const K key, const V value, int level);
    int GetRandomLevel() const;

private:
    // maximum skip list level
    int max_level_;
    int current_level_;
    Node<K, V> *header_;
    // store element count
    int element_count_;
    std::mutex mutex_;  // for operation lock
};

template <typename K, typename V>
SkipList<K, V>::SkipList(int max_level) {
    max_level_ = max_level;
    current_level_ = 0;
    element_count_ = 0;

    // create empty header node
    K k;
    V v;
    header_ = new Node<K, V>(k, v, max_level_);
}

template <typename K, typename V>
SkipList<K, V>::~SkipList() {
    delete header_;
    header_ = nullptr;
}

/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4  header-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+
*/
template <typename K, typename V>
bool SkipList<K, V>::InsertElement(K key, V value) {
    std::lock_guard<std::mutex> guard(mutex_);

    auto current = header_;

    // 找到每层的最后一个前置节点
    Node<K, V> *update[max_level_];
    std::memset(update, 0, sizeof(Node<K, V> *) * max_level_);

    // find and init update array
    for (int i = current_level_; i >= 0; i--) {
        while (current->levels_[i] != nullptr &&
               current->levels_[i]->GetKey() < key) {
            current = current->levels_[i];
        }
        update[i] = current;
    }

    // reached level 0 and pointer to insert key position
    current = current->levels_[0];

    // not support same key just return false
    if (current != nullptr && current->GetKey() == key) {
        return false;
    }

    if (current == nullptr || current->GetKey() != key) {
        int random_level = GetRandomLevel();

        if (random_level > current_level_) {
            for (int i = current_level_ + 1; i < random_level + 1; i++) {
                update[i] = header_;
            }
            current_level_ = random_level;
        }

        // 每层将新节点插入前继节点后面
        Node<K, V> *newNode = CreateNode(key, value, random_level);

        for (int i = 0; i <= random_level; i++) {
            newNode->levels_[i] = update[i]->levels_[i];
            update[i]->levels_[i] = newNode;
        }
        
        element_count_++;
    }

    return true;
}

// Search for element in skip list 
/*
                           +------------+
                           |  search 60 |
                           +------------+
level 4  header-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template<typename K, typename V>
bool SkipList<K, V>::SearchElement(const K key, V &value) {
    auto current = header_;
   
    // 从最高level查找每层最后一个小于key的节点
    for (int i = current_level_; i >= 0; i--) {
        while(current->levels_[i] != nullptr && current->levels_[i]->GetKey() < key) {
            current = current->levels_[i];
        }
    }
    // 
    current = current->levels_[0];

    if (current != nullptr && current->GetKey() == key) {
        value = current->GetValue();
        return true;
    }
    return false;
}

template<typename K, typename V>
Node<K, V> *SkipList<K, V>::CreateNode(const K key, const V value, int level) {
    return new Node<K, V>(key, value, level);
}

template <typename K, typename V>
int SkipList<K, V>::GetRandomLevel() const {
    int k = 1;
    while (rand() % 2 != 0) {
        k++;
    }
    k = k < max_level_ ? k : max_level_;
    return k;
}

#endif  // TBAGO_SKIP_LIST_HPP_
