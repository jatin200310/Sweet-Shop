#include "JWT.h"
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <cstring>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include<vector>

JWT::JWT(const std::string& secret)
    : secret_(secret) {}

std::string JWT::base64UrlEncode(const std::string& input) const {
    static const char base64_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    
    std::string out;
    int val = 0, valb = 0;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 6) {
            valb -= 6;
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
        }
    }
    if (valb > 0) {
        out.push_back(base64_chars[(val << (6 - valb)) & 0x3F]);
    }
    // Remove padding
    while (out.size() % 4) out.push_back('=');
    return out;
}

std::string JWT::base64UrlDecode(const std::string& input) const {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;
    
    int val = 0, valb = 0;
    for (unsigned char c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 8) {
            valb -= 8;
            out.push_back(char((val >> valb) & 0xFF));
        }
    }
    return out;
}

std::string JWT::hmacSha256(const std::string& data) const {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;
    
    HMAC(EVP_sha256(),
         reinterpret_cast<const unsigned char*>(secret_.c_str()),
         secret_.size(),
         reinterpret_cast<const unsigned char*>(data.c_str()),
         data.size(),
         hash,
         &hashLen);
    
    return std::string(reinterpret_cast<char*>(hash), hashLen);
}

std::string JWT::encode(const std::map<std::string, std::string>& claims,
                        unsigned int expirySeconds) const {
    // Header: {"alg":"HS256","typ":"JWT"}
    std::string header = R"({"alg":"HS256","typ":"JWT"})";
    std::string encodedHeader = base64UrlEncode(header);

    // Payload with claims + exp (if expirySeconds > 0)
    std::ostringstream payloadStream;
    payloadStream << "{";
    bool first = true;
    for (const auto& kv : claims) {
        if (!first) payloadStream << ",";
        payloadStream << "\"" << kv.first << "\":\"" << kv.second << "\"";
        first = false;
    }
    if (expirySeconds > 0) {
        if (!first) payloadStream << ",";
        time_t now = std::time(nullptr);
        time_t exp = now + expirySeconds;
        payloadStream << "\"exp\":" << exp;
    }
    payloadStream << "}";
    
    std::string payload = payloadStream.str();
    std::string encodedPayload = base64UrlEncode(payload);

    // Signature
    std::string signatureInput = encodedHeader + "." + encodedPayload;
    std::string signature = hmacSha256(signatureInput);
    std::string encodedSignature = base64UrlEncode(signature);

    return signatureInput + "." + encodedSignature;
}

std::map<std::string, std::string> JWT::decode(const std::string& token) const {
    std::map<std::string, std::string> claims;
    
    // Split token by '.'
    size_t pos1 = token.find('.');
    if (pos1 == std::string::npos) return claims;
    size_t pos2 = token.find('.', pos1 + 1);
    if (pos2 == std::string::npos) return claims;

    std::string encodedPayload = token.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string payload = base64UrlDecode(encodedPayload);

    // Very simple JSON parser for flat key-value pairs
    size_t idx = 0;
    while ((idx = payload.find('"', idx)) != std::string::npos) {
        size_t keyStart = idx + 1;
        size_t keyEnd = payload.find('"', keyStart);
        if (keyEnd == std::string::npos) break;
        std::string key = payload.substr(keyStart, keyEnd - keyStart);
        
        size_t colonPos = payload.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valStart = payload.find_first_not_of(" \t", colonPos + 1);
        if (valStart == std::string::npos) break;
        
        std::string value;
        if (payload[valStart] == '"') {
            valStart++;
            size_t valEnd = payload.find('"', valStart);
            if (valEnd != std::string::npos) {
                value = payload.substr(valStart, valEnd - valStart);
            }
        } else {
            size_t valEnd = payload.find_first_of(",}", valStart);
            if (valEnd != std::string::npos) {
                value = payload.substr(valStart, valEnd - valStart);
            }
        }
        
        claims[key] = value;
        idx = keyEnd + 1;
    }
    
    return claims;
}

bool JWT::verify(const std::string& token) const {
    // Split token
    size_t pos1 = token.find('.');
    if (pos1 == std::string::npos) return false;
    size_t pos2 = token.find('.', pos1 + 1);
    if (pos2 == std::string::npos) return false;

    std::string signatureInput = token.substr(0, pos2);
    std::string providedSignature = token.substr(pos2 + 1);

    // Recompute signature
    std::string computedSig = hmacSha256(signatureInput);
    std::string encodedComputedSig = base64UrlEncode(computedSig);

    // Compare (simple string compare; in production use constant-time)
    if (encodedComputedSig != providedSignature) {
        return false;
    }

    // Check expiry if present
    auto claims = decode(token);
    if (claims.count("exp")) {
        try {
            time_t exp = std::stol(claims["exp"]);
            time_t now = std::time(nullptr);
            if (now > exp) {
                return false; // Token expired
            }
        } catch (...) {
            return false;
        }
    }

    return true;
}