#include <iostream>
#include <fstream>
#include <string>
#include "../src/crypto/LamportChain.h"

using namespace std;

string readFile(const string &path) {
    ifstream fin(path);
    if (!fin.is_open()) {
        cerr << "Error: cannot open file " << path << endl;
        exit(1);
    }
    string content;
    getline(fin, content);
    fin.close();
    return content;
}

int main() {
    // Load values
    string h0 = readFile("bob_h0.txt");
    string hn = readFile("alice_hn.txt");


    int n = 100;

    // Compute H^n(h0)
    string computed_hn = LamportChain::HashRepeat(h0, n);

    cout << "Loaded h0 = " << h0 << endl;
    cout << "Loaded hn = " << hn << endl;
    cout << "Computed hn = " << computed_hn << endl;

    if (computed_hn == hn) {
        cout << "[PASS] Chain verification successful!" << endl;
    } else {
        cout << "[FAIL] Computed hn does not match stored hn." << endl;
    }

    // Test VerifyResponse for one round:
    // Suppose Alice challenges with c=1
    // Bob responds with r = H^(n-1)(h0)
    string r = LamportChain::HashRepeat(h0, n - 1);
    bool ok = LamportChain::VerifyResponse(r, hn);
    cout << "VerifyResponse (round 1): " << (ok ? "PASS" : "FAIL") << endl;

    return 0;
}
