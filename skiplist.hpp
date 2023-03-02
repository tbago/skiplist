#ifndef TBAGO_SKIP_LIST_HPP_
#define TBAGO_SKIP_LIST_HPP_

// skiplist kv store node
template<typename K, typename V>
class Node {
public:
    explicit Node(K key, V value, int level) : key_(key), value_(value),level_(level) {}
    K GetKey() const {return key_; }
    V GetValue() const {return value_; }
    void SetValue(V value) {value_ = value;}
private:
    K key_;
    V value_;
    int level_;
};  // class Node

#endif // TBAGO_SKIP_LIST_HPP_
