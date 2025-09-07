#include <iostream>
#include <fstream>
#include <string>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>

using namespace CryptoPP;
using namespace std;

string sha256(const string &input) {
    SHA256 hash;
    string digest;
    StringSource(input, true,
        new HashFilter(hash,
            new HexEncoder(
                new StringSink(digest), false // false = lowercase
            )
        )
    );
    return digest;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <seed> [n=100]" << endl;
        return 1;
    }

    string seed = argv[1];
    int n = (argc >= 3) ? stoi(argv[2]) : 100;

    string h = seed;
    ofstream fout("hash_chain.txt");

    fout << "h0 (seed): " << h << endl;
    for (int i = 1; i <= n; i++) {
        h = sha256(h);
        fout << "h" << i << ": " << h << endl;
    }

    fout.close();

    cout << "Hash chain generated (length " << n << ")" << endl;
    cout << "Bob stores h0 = " << seed << endl;
    cout << "Alice stores hn = " << h << endl;
    cout << "Full chain written to hash_chain.txt" << endl;

    return 0;
}
