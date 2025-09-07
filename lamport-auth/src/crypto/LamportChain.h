#ifndef LAMPORTCHAIN_H
#define LAMPORTCHAIN_H

#include <string>

class LamportChain
{
public:
    // Hash once: input is an ASCII string (e.g., hex string or seed),
    // returns lowercase hex digest of SHA-256(input).
    static std::string HashOnce(const std::string &input);

    // Hash repeatedly: apply HashOnce 'times' times.
    static std::string HashRepeat(const std::string &input, int times);

    // Verify response r against expectedHash: returns true if HashOnce(r) == expectedHash
    static bool VerifyResponse(const std::string &r, const std::string &expectedHash);
};

#endif // LAMPORTCHAIN_H
