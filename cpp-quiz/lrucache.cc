#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;

class LRUCache {
public:
  using store_t = list<pair<int, int>>;

  store_t store;
  unordered_map<int, store_t::iterator> cache;
  size_t capacity;

  LRUCache(size_t c) : capacity(c) {}

  bool is_full() { return store.size() == capacity; }

  int get(int key) {
    auto it = cache.find(key);
    if (it == cache.end()) {
      return -1;
    }
    // move to front
    int value = it->second->second;
    store.erase(it->second);
    store.push_front({key, value});
    cache[key] = store.begin();
    return value;
  }

  void put(int key, int value) {
    auto it = cache.find(key);
    if (it == cache.end()) {
      if (is_full()) {
        auto last = --store.end();
        cout << "remove:" << last->first << endl;
        cache.erase(last->second);
        store.pop_back();
      }
      cout << "create:" << key << endl;
      store.push_back({key, value});
      cache[key] = --store.end();
    } else {
      // update value onely
      it->second->second = value;
    }
  }
};

int main() {
  LRUCache cache(2);
  cout << cache.get(1) << endl;
  cache.put(1, 1);
  cout << cache.get(1) << endl;
  cache.put(2, 2);
  cout << cache.get(2) << endl;
  // remove 1
  // create 3
  cache.put(3, 3);
  // -1
  cout << cache.get(1) << endl;
  // 2
  cout << cache.get(2) << endl;
  // remove 3
  // create 4
  cache.put(4, 4);
  // -1
  cout << cache.get(3) << endl;
  // 4
  cout << cache.get(4) << endl;
}