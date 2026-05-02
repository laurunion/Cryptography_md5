#include <iostream>
#include <iomanip>
#include <sstream>
#include <CommonCrypto/CommonDigest.h>
using namespace std;

string md5(string input) {
    unsigned char digest[CC_MD5_DIGEST_LENGTH];

    CC_MD5(input.c_str(), input.length(), digest);

    stringstream ss;
    for (int i = 0; i < CC_MD5_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)digest[i];
    }

    return ss.str();
}

int main() {
    string text;

    cout << "Enter text: ";
    getline(cin, text);

    cout << "MD5 Hash: " << md5(text) << endl;
    cout << "Hash length: " << md5(text).length() << " characters" << endl;

    return 0;
}