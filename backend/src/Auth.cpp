#include "Auth.h"
#include "Database.h"
#include "JWT.h"

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

Auth::Auth(Database& db, const std::string& jwtSecret)
    : db_(db), secret_(jwtSecret) {}

std::string Auth::hashPassword(const std::string& plaintext) const {
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(reinterpret_cast<const unsigned char*>(plaintext.c_str()),
           plaintext.size(),
           hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2)
           << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool Auth::verifyPassword(const std::string& plaintext,
                          const std::string& hash) const {
    return hashPassword(plaintext) == hash;
}

std::string Auth::createToken(
    const std::map<std::string, std::string>& claims) const {
    JWT jwt(secret_);
    return jwt.encode(claims, 604800); // 7 days
}

std::string Auth::registerUser(const std::string& username,
                               const std::string& password,
                               const std::string& email) {
    if (username.empty() || password.empty() || email.empty()) {
        return "";
    }

    auto existing = db_.getUserByUsername(username);
    if (!existing.empty()) {
        return "";
    }

    std::string hash = hashPassword(password);
    if (hash.empty()) {
        return "";
    }

    if (!db_.createUser(username, hash, email, false)) {
        return "";
    }

    std::map<std::string, std::string> claims;
    claims["username"] = username;
    claims["email"] = email;
    claims["is_admin"] = "false";

    return createToken(claims);
}

std::string Auth::login(const std::string& username,
                        const std::string& password) {
    if (username.empty() || password.empty()) {
        return "";
    }

    auto user = db_.getUserByUsername(username);
    if (user.empty()) {
        return "";
    }

    if (!verifyPassword(password, user["password_hash"])) {
        return "";
    }

    std::map<std::string, std::string> claims;
    claims["username"] = user["username"];
    claims["email"] = user.count("email") ? user["email"] : "";
    claims["is_admin"] = user.count("is_admin") ? user["is_admin"] : "false";
    claims["user_id"] = user.count("id") ? user["id"] : "0";

    return createToken(claims);
}

bool Auth::validateToken(const std::string& token) const {
    JWT jwt(secret_);
    return jwt.verify(token);
}

std::map<std::string, std::string>
Auth::decodeToken(const std::string& token) const {
    JWT jwt(secret_);
    return jwt.decode(token);
}
