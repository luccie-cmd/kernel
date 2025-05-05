#if !defined(_ASSEMBLER_ORDERED_MAP_H_)
#define _ASSEMBLER_ORDERED_MAP_H_
#include <utility>
#include <vector>
#include <cstdio>

template <typename Key, typename Value>
class OrderedMap
{
  public:
    OrderedMap(std::initializer_list<std::pair<Key, Value>> pairs){
        this->_M_values = pairs;
    }
    void insert_or_assign(Key k, Value v)
    {
        for (std::pair<Key, Value>& kvPair : _M_values)
        {
            if (kvPair.first == k)
            {
                kvPair.second = v;
                return;
            }
        }
        _M_values.push_back({k, v});
    }
    Value& at(Key k) {
        for (std::pair<Key, Value>& kvPair : _M_values)
        {
            if (kvPair.first == k)
            {
                return kvPair.second;
            }
        }
        std::puts("Out of range error\n");
        std::abort();
    }
    std::pair<Key, Value> at(size_t index){
        return _M_values.at(index);
    }
    size_t size(){
        return _M_values.size();
    }
    bool contains(Key k) {
        for (std::pair<Key, Value>& kvPair : _M_values)
        {
            if (kvPair.first == k)
            {
                return true;
            }
        }
        return false;
    }
    auto begin(){
        return _M_values.begin();
    }
    auto end(){
        return _M_values.end();
    }

  private:
    std::vector<std::pair<Key, Value>> _M_values;
};

#endif // _ASSEMBLER_ORDERED_MAP_H_
