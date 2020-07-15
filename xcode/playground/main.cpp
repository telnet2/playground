//
//  main.cpp
//  playground
//
//  Created by Joohwi Lee on 8/2/19.
//  Copyright © 2019 Joohwi Lee. All rights reserved.
//

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

static unsigned char charToInt(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

vector<unsigned char> hex2bytes(const string& hex) {
    vector<unsigned char> out;
    auto inserter = back_inserter(out);
    for (auto iter = begin(hex); iter != end(hex); iter += 2) {
        unsigned char x = (charToInt(*iter) << 4) + charToInt(*(iter+1));
        *inserter = x;
        ++inserter;
    }
    return out;
}

string bytes2hex(const vector<unsigned char>& vec) {
    stringstream ss;
    for(int i = 0; i < vec.size(); ++i)
        ss << setw(2) << setfill('0') << std::hex << (int)vec[i];
    return ss.str();
}

vector<unsigned char> one_time_pad(const vector<unsigned char>& key, const vector<unsigned char>& pt) {
    vector<unsigned char> ct;
    for (auto iter1 = begin(key), iter2 = begin(pt); iter1 != end(key) && iter2 != end(pt); ++iter1, ++iter2) {
        ct.push_back(*iter1 ^ *iter2);
    }
    return ct;
}

int quiz1(int argc, const char * argv[]) {
    string msg = "attack at dawn";
    vector<unsigned char> pt{begin(msg), end(msg)};
    
    cout << "Plain Text: " << bytes2hex(pt) << endl;
    
    string hex = "09e1c5f70a65ac519458e7e53f36";
    vector<unsigned char> ct = hex2bytes(hex);
    cout << "Cipher Text: " << bytes2hex(ct) << endl;

    const auto key = one_time_pad(pt, ct);
    cout << "Key: " << bytes2hex(key) << endl;
    
    const string msg2 = "attack at dusk";
    cout << "CT: " << bytes2hex(one_time_pad(key, vector<unsigned char>{begin(msg2), end(msg2)})) << endl;
    return 0;
}

string bytes2ascii(const vector<unsigned char>& bytes) {
    return string(begin(bytes), end(bytes));
}

void quiz2() {
    string ct1 = "32510bfbacfbb9befd54415da243e1695ecabd58c519cd4bd2061bbde24eb76a19d84aba34d8de287be84d07e7e9a30ee714979c7e1123a8bd9822a33ecaf512472e8e8f8db3f9635c1949e640c621854eba0d79eccf52ff111284b4cc61d11902aebc66f2b2e436434eacc0aba938220b084800c2ca4e693522643573b2c4ce35050b0cf774201f0fe52ac9f26d71b6cf61a711cc229f77ace7aa88a2f19983122b11be87a59c355d25f8e4";
    
    string ct2 = "32510bfbacfbb9befd54415da243e1695ecabd58c519cd4bd90f1fa6ea5ba47b01c909ba7696cf606ef40c04afe1ac0aa8148dd066592ded9f8774b529c7ea125d298e8883f5e9305f4b44f915cb2bd05af51373fd9b4af511039fa2d96f83414aaaf261bda2e97b170fb5cce2a53e675c154c0d9681596934777e2275b381ce2e40582afe67650b13e72287ff2270abcf73bb028932836fbdecfecee0a3b894473c1bbeb6b4913a536ce4f9b13f1efff71ea313c8661dd9a4ce";
    
    const auto key = one_time_pad(hex2bytes(ct1), hex2bytes(ct2));
    
    cout << "Key: " << bytes2hex(key) << endl;
    string msg = "32510ba9babebbbefd001547a810e67149caee11d945cd7fc81a05e9f85aac650e9052ba6a8cd8257bf14d13e6f0a803b54fde9e77472dbff89d71b57bddef121336cb85ccb8f3315f4b52e301d16e9f52f904";
    cout << bytes2hex(one_time_pad(key, hex2bytes(ct1))) << endl;
}

int main(int argc, const char* argv[]) {
    quiz2();
    return 0;
}
