//
//  util.cpp
//  cxx17
//
//  Created by Joohwi Lee on 2/18/20.
//  Copyright © 2020 Joohwi Lee. All rights reserved.
//
#include <iostream>
#include <utility>

#include "util.hpp"

using namespace std;

void test_pair() {
    pair x(1,2);
    cout << x.first << ", " << x.second << endl;
}
