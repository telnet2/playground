//
//  main.cpp
//  boost_assign
//
//  Created by Joohwi Lee on 8/2/19.
//  Copyright © 2019 Joohwi Lee. All rights reserved.
//

#include <iostream>
#include <vector>
#include <list>

using namespace std;

int main(int argc, const char * argv[]) {
    list<int> l{1,2,3,4};
    
    for (auto i: l) {
        cout << i << endl;
    }
    
    auto iter = l.begin();
    auto first = l.begin();
    
    cout << "first: " << *iter << endl;
    ++iter;
    cout << "first: " << *first << endl;
    cout << "second: " << *iter << endl;
    
    auto new_end = remove(begin(l), end(l), 1);
    
    cout << "sizeof L: " << l.size() << endl;
    l.erase(new_end, end(l));
    cout << "sizeof L: " << l.size() << endl;
    
    // The content of iter has chagned to 2
    cout << "first: " << *first << endl;
    cout << "iter: " << *iter << endl;
    return 0;
}
