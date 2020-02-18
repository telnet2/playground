//
//  main.cpp
//  lru_cache
//
//  Created by Joohwi Lee on 2/16/20.
//  Copyright © 2020 Joohwi Lee. All rights reserved.
//

#include <iostream>
#include <unordered_map>
#include <list>
#include <tuple>

using namespace std;

class LRUCache {
public:
    LRUCache(int size): _size(size), _count(0) {}
    
    int size() { return _size; }
    
    void put(int e) {
        if (_count < _size) {
            _cache.push_back(e);
        }
    }
    
    void get(int e) {
        
    }
    
private:
    int _count;
    int _size;
    list<int> _cache;
    unordered_map<int, list<int>::iterator> _index;
};

int main(int argc, const char * argv[]) {
    pair x(1,2);
    return 0;
}
