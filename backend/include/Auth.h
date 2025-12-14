#ifndef SWEET_SHOP_AUTH_H
#define SWEET_SHOP_AUTH_H

#include <string>
#include <map>

class Database; // forward declaration

class Auth {
public:
    Auth(Database& db, const std::string& jwtSecret);

    std::string registerUser(const std::string& username,
                             const std::string& password,
                             const std::string& email);

    std::string login(const std::string& username,
                      const std::string& password);

    bool validateToken(const std::string& token) const;

    std::map<std::string, std::string>
    decodeToken(const std::string& token) const;

private:
    Database& db_;
    std::string secret_;

    std::string hashPassword(const std::string& plaintext) const;
    bool verifyPassword(const std::string& plaintext,
                        const std::string& hash) const;
    std::string createToken(
        const std::map<std::string, std::string>& claims) const;
};

#endif // SWEET_SHOP_AUTH_H
