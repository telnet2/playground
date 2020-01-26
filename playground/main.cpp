//
//  main.cpp
//  playground
//
//  Created by Joohwi Lee on 8/2/19.
//  Copyright © 2019 Joohwi Lee. All rights reserved.
//

#include <iostream>
#include <boost/assign/std/vector.hpp>

int main(int argc, const char * argv[]) {
    using namespace std;
    using namespace boost::assign;
    
    vector<int> values;
    values += 1,2,3,4,5,6,7,8,9;
    
    cout << "# of values: " << values.size() << endl;
    for (const auto v: values) {
        cout << v << endl;
    }
    return 0;
}
