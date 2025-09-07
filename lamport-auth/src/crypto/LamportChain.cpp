#include "LamportChain.h"
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <cryptopp/strciphr.h>
#include <string>

using namespace CryptoPP;

std::string LamportChain::HashOnce(const std::string &input)
{
    // Hash the raw bytes of the input string (ASCII). Output hex (lowercase).
    std::string digest;
    SHA256 hash;
    StringSource ss(input, true,
        new HashFilter(hash,
            new HexEncoder(new StringSink(digest), false) // false -> lowercase
        )
    );
    return digest;
}

std::string LamportChain::HashRepeat(const std::string &input, int times)
{
    std::string cur = input;
    for (int i = 0; i < times; ++i)
    {
        cur = HashOnce(cur);
    }
    return cur;
}

bool LamportChain::VerifyResponse(const std::string &r, const std::string &expectedHash)
{
    // expectedHash is the stored value on Alice (hn initially, then updated)
    std::string hashed = HashOnce(r);
    return (hashed == expectedHash);
}
