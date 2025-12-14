#ifndef SWEET_SHOP_JWT_H
#define SWEET_SHOP_JWT_H

#include <string>
#include <map>

class JWT {
public:
    // Construct with a secret used for signing tokens
    explicit JWT(const std::string& secret);

    // Create a compact JWT. expirySeconds = 0 means no exp claim.
    std::string encode(const std::map<std::string, std::string>& claims, unsigned int expirySeconds = 0) const;

    // Decode a token. Returns an empty map on failure.
    std::map<std::string, std::string> decode(const std::string& token) const;

    // Verify signature and expiry. Returns true if valid.
    bool verify(const std::string& token) const;

private:
    std::string secret_;

    // Helpers (implementation details in JWT.cpp)
    std::string base64UrlEncode(const std::string& input) const;
    std::string base64UrlDecode(const std::string& input) const;
    std::string hmacSha256(const std::string& data) const;
};

#endif // SWEET_SHOP_JWT_H